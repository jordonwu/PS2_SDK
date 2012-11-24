/*
    -----------------------------------------------------------------------
    mp3play.cpp - PS2MP3. (c) Ryan Kegel, 2004
	-----------------------------------------------------------------------

    This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

*/
#include "tamtypes.h"
#include <stdio.h>
#include <sifrpc.h>
#include <sifcmd.h>
#include "sys/stat.h"
#include "sys/fcntl.h"
#include "kernel.h"
#include "sifrpc.h"
#include "stdarg.h"
#include "iopheap.h"
#include "sys/ioctl.h"
#include "fileXio_rpc.h"
#include "errno.h"

#include "id3tag.h"
#include "string.h"
#include "fileio.h"
#include "file.h"
#include "debug.h"
#include "sjpcm.h"
#include "bstdfile.h"
#include "mad.h"
#include "sample.h"
#include "rmalloc.h"
#include "mp3help.h"
#include "libmp3.h"
#include "cdvd_rpc.h"


#define SAMPLEBUFFER 250000*sizeof(short) //this is the size of our resampling buffer
#define TICK 960  //800 for NTSC,  960 for PAL

static short left[TICK*3] __attribute__((aligned (64)));  //Holds the samples we put into the hardware
static short right[TICK*3] __attribute__((aligned (64)));

static int iSamples;
int totalStreams = 0;
short Sample;
static short *sBuffer1, *sBuffer2, *sampleBuffer, *resampleBuffer;  //All the buffers we will be using
static short *sBufferList[2];
int	   sBufferLength[2];
int    currentSwap = 0;
int    bufferSwap = 0;
char   buffering = 0;

int PLAYBUFFER =  260000*sizeof(short);  //This is the size of our playbuffer.

//definitions for dlanor's logarithmic volume control
#define MaxVBits			14
#define MaxVLimit			0x4000
#define	MaxVolume			0x3fff
#define	MaxVplus1dB		0x47cf
#define	MaxVminus1dB	0x390a
#define	NormVolume		0x3fff
//definitions for modes (DB = dnalor's logarithmic volume control 
//                       LINEAR = stefy2's linear volume control)
#define DBMODE 0
#define LINEARMODE 1


int volume_tmp;
int volume = NormVolume;
int volumemode = DBMODE; //default mode dnalor's logarithmic volume control
//char volumetxt[50] = "0"

//define for repeat function
#define REPEATON 1
#define REPEATOFF 0
#define REPEATALL 2

int Repeat = REPEATOFF;

char info[600]; //for printheaderinformation
char infodata[600]; //mantain on memory info data
char repeatinfo[10]; //information about repeat presence
char volumeinfo[50]; //information about volume


int		breakList;

static char userThreadStack[16*1024] __attribute__((aligned(16)));
extern void *_gp;
int pid = 0;
int mainPid = 0;
char preBuffer = 1;
long totalSamples = 0;
long readSamples = 0;
//gsDriver *gDrv;




ls_sample_type_t in_type;
ls_sample_type_t out_type;
ls_resampler_t *resampler;
unsigned int sampleSize;
void *state;
char currentTitle[500];
unsigned int oldRate;
char resamplerInMemory;
char breakOut;



#define csr			0x12001000	// System status and reset
#define CSR			((volatile u64 *)(csr))


struct audio_dither 
{
    mad_fixed_t error[3];
    mad_fixed_t random;
};

/****************************************************************************
 * Waits one vertical blank.												*
 ****************************************************************************/
inline void wait_vsync(void)
{
	*CSR = *CSR & 8;
	while(!(*CSR & 8));
}


/****************************************************************************
 * Used with bstdfile.														*
 ****************************************************************************/
#define INPUT_BUFFER_SIZE	(5*8192)
#define OUTPUT_BUFFER_SIZE	8192 /* Must be an integer multiple of 4. */



char *control;
int iFile;

/****************************************************************************
 * Reads a text line from a file to a buffer, removes trailing control chars*
 * and returns the remaining length of the line (Zero if none could be read)*
 ****************************************************************************/
int ReadLine(int handle, char *buf)
{	int i, sr;

	for	(i=0; i<510; i++)
	{	sr = ReadFile(handle,(unsigned char *) &(buf[i]), 1, mediaMode);
		if	((sr != 1) || (buf[i] == '\n'))
			break;
	}
	buf[i] = 0;
	while	((i = strlen(buf)) > 0)
	{	if	(buf[i-1] > 0x20)
			break;
		buf[i-1] = 0;
	}
	return strlen(buf);
}

/****************************************************************************
 * Plays the MP3's of an M3U playlist specified by 'filename.'							*
 * 'pad' and 'D' are pointers to our graphics driver and control pad.				*
 ****************************************************************************/
int PlayM3U(char *filename, functionPointer callback)
{	char	PL_Line[512];
	int PL_ct, PL_File;
	// int sr;

	PL_File = OpenFile(filename, O_RDONLY, mediaMode);
	if	(PL_File < 0)
		return 1;
	breakList = 0;
	PL_ct = 0;
	while	((breakList == 0) && (ReadLine(PL_File, PL_Line) > 0))
	{	if	(PL_Line[0] != '#')
		{	PL_ct++;
			PlayMP3(PL_Line, callback);
		}
	}
	CloseFile(PL_File, mediaMode);
	while (Repeat == REPEATALL)
	{	
		PL_File = OpenFile(filename, O_RDONLY, mediaMode);
	if	(PL_File < 0)
		return 1;
	breakList = 0;
	PL_ct = 0;
	while	((breakList == 0) && (ReadLine(PL_File, PL_Line) > 0))
	{	if	(PL_Line[0] != '#')
		{	PL_ct++;
			PlayMP3(PL_Line, callback);
		}
	}
	CloseFile(PL_File, mediaMode);
	}
	breakList = 1;
	
	return 1;
}


functionPointer cb __attribute__((aligned (16)));


/****************************************************************************
 * Plays a specific MP3 from 'filename.'									*
 ****************************************************************************/
int PlayMP3(char *filename, functionPointer callback)
{
	int i;
	int len = strlen(filename);

//	printf("\nPlayMP3(%s) \n", filename);
	cb = callback;

	iFile = OpenFile(filename, O_RDONLY, mediaMode);
	for (i=len-1; i!=0; i--)
	{
		if (filename[i] == '/')
			break;
	}
	i++;
	strcpy(currentTitle, &filename[i]);
	len = strlen(currentTitle);
	currentTitle[len-4] = '\0';

//	printf("PlayMP3(%s) executing MpegAudioDecoder \n", filename);

	MpegAudioDecoder(iFile);

//	printf("PlayMP3(%s) executing CloseFile \n", filename);

	CloseFile(iFile, mediaMode);
	
	//repeat the song as far as the user exit the execution of it or 
	//disable the repeat function
	while (Repeat == REPEATON)
	{
		int i;
		int len = strlen(filename);
		iFile = OpenFile(filename, O_RDONLY, mediaMode);
		for (i=len-1; i!=0; i--)
		{
			if (filename[i] == '/')
				break;
		}
		i++;
		strcpy(currentTitle, &filename[i]);
		len = strlen(currentTitle);
		currentTitle[len-4] = '\0';
		MpegAudioDecoder(iFile);
		CloseFile(iFile, mediaMode);
	}

//	printf("end of PlayMP3(%s) \n", filename);
	
	return 1;
}

int position = 0;
/****************************************************************************
 * The threaded function Play will take samples from PLAYBUFFER and place	*
 * them into the hardware queue.  Once one PLAYBUFFER is empty, it request  *
 * that the next PLAYBUFFER be filled.										*
 ****************************************************************************/
void Play()
{
	iSamples = 0;
	position = 0;
	memset(left, 0, TICK*3);
	memset(right, 0, TICK*3);
	SleepThread();
	SjPCM_Clearbuff();
//	printf ("Play()");
	while (1)
	{
		while (position < (PLAYBUFFER/4))  //Begin to play 1 buffer
		{
			left[iSamples] = sBufferList[currentSwap][(2*position)];
			right[iSamples] = sBufferList[currentSwap][(2*position)+1];
			position++;
			iSamples++;
			totalSamples++;
		
			/*They say to keep TICK 800, but underrun occurs, so I use 800*3 */
			if (iSamples == TICK*3)  //If iSamples are a multiple of 800...
			{
				if (SjPCM_Available() >= TICK*3) 
				{
					SjPCM_Enqueue(left, right, TICK*3, 0); // avoid underrun
					wait_vsync();
					iSamples = 0;
				}
				else
				{
					position--; iSamples--; totalSamples--;
				}
			}
			if ( (totalSamples >= readSamples) || breakOut) //The song is over or Triangle was hit
			{
				printf("Song done\n");
				position = 0;
				while (position == 0)
				{
					position++;
					if (SjPCM_Buffered() > TICK)
					{
						wait_vsync();
						position--;
					}
				}
				SjPCM_Clearbuff();
				if (breakOut)
					currentSwap = bufferSwap = position = iSamples = 0;
				WakeupThread(mainPid);
				return;
			}

		}
		//The current buffer is empty now.  Request it be filled and start playing
		//the new buffer.
		position = 0;
		if (currentSwap == 1)
		{
			currentSwap = 0; bufferSwap = 1;
		}
		else if (currentSwap == 0)
		{
			currentSwap = 1; bufferSwap = 0;
		}
		if (preBuffer == 2)
		{
			WakeupThread(mainPid);
		}
	}	//ends while(1)
}


/****************************************************************************
 * This is the libmad MP3 decoder.  This will decode an MP3 file based off	*
 * a file handle 'InputFP.'  It will fill the buffers and created the		*
 * threaded function Play().												*
 ****************************************************************************/
int MpegAudioDecoder(int InputFp)
{
	struct mad_stream	Stream;
	struct mad_frame	Frame;
	struct mad_synth	Synth;
	mad_timer_t			Timer;
	unsigned char		InputBuffer[INPUT_BUFFER_SIZE+MAD_BUFFER_GUARD],
						*GuardPtr=NULL;
	int					Status=0,
						i;
	unsigned long		FrameCount=0;
	bstdfile_t			*BstdFile;
	int					bufferIndex = 0;
	int					status;
	struct audio_dither	left_dither, right_dither;

	/* First the structures used by libmad must be initialized. */
	mad_stream_init(&Stream);
	mad_frame_init(&Frame);
	mad_synth_init(&Synth);
	mad_timer_reset(&Timer);
	
	ee_thread_t thread;
	
	thread.func = (void *)Play;
	thread.stack = userThreadStack;
	thread.stack_size = sizeof(userThreadStack);
	thread.gp_reg = &_gp;
	thread.initial_priority = 48;

	totalStreams = 0;
	currentSwap = 0;
	bufferSwap = 0;
	pid = 0;
	mainPid = 0;
	preBuffer = 1;
	totalSamples = 0;
	readSamples = 0;
	position = 0;

	/*Allocate memory for the buffers */
	sBuffer1 = (short *)rmalloc(PLAYBUFFER);
	memset(sBuffer1, 0, PLAYBUFFER);
	sBuffer2 = (short *)rmalloc(PLAYBUFFER);
	memset(sBuffer2, 0, PLAYBUFFER);
	sampleBuffer = (short *)rmalloc(SAMPLEBUFFER);
	memset(sampleBuffer, 0, SAMPLEBUFFER);
	resampleBuffer = (short *)rmalloc(SAMPLEBUFFER);
	memset(resampleBuffer, 0, SAMPLEBUFFER);

	in_type.rate = 0;
	resamplerInMemory = 0;

	sBufferList[0] = sBuffer1;
	sBufferList[1] = sBuffer2;
	sBufferLength[0] = sBufferLength[1] = 0;
	preBuffer = 1;
	bufferIndex = 0;
	
	in_type.channels = 2;
	in_type.bits = 16;
	in_type.enc = LS_SIGNED;
	in_type.be = 0;
	/*The resampler will resample to this format */
	out_type.rate = 48000;
	out_type.channels = 2;
	out_type.bits = 16;
	out_type.enc = LS_SIGNED;
	out_type.be = 0;

	breakOut = 0;
	
	pid = CreateThread(&thread);
	mainPid = GetThreadId();
	status = StartThread(pid, NULL);

	/* Decoding options can here be set in the options field of the
	 * Stream structure.
	 */

	/* {1} When decoding from a file we need to know when the end of
	 * the file is reached at the same time as the last bytes are read
	 * (see also the comment marked {3} bellow). Neither the standard
	 * C fread() function nor the POSIX read() system call provides
	 * this feature. We thus need to perform our reads through an
	 * interface having this feature, this is implemented here by the
	 * bstdfile.c module.
	 */
	//BstdFile=NewBstdFile(InputFp, mediaMode);
	if (mediaMode == MODE_HOST)
	{
		BstdFile=NewBstdFile(InputFp, mediaMode);
	}
	else
		BstdFile=NewBstdFile(InputFp, 0);
	if(BstdFile==NULL)
	{
		printf("could not open file\n");		
		return(1);
	}

	/*this will stop the DVD drive since we've already read the file to memory */
	if ( (mediaMode == MODE_CD) && isMemoryFile(BstdFile))
		CDVD_Stop();
	/* This is the decoding loop. */
	do
	{
		if( breakOut )  //If Triangle was in in the Play() function
		{
			SjPCM_Play();
			break;
		}
		/* The input bucket must be filled if it becomes empty or if
		 * it's the first execution of the loop.
		 */
		if(Stream.buffer==NULL || Stream.error==MAD_ERROR_BUFLEN)
		{
			size_t			ReadSize,
							Remaining;
			unsigned char	*ReadStart;

			/* {2} libmad may not consume all bytes of the input
			 * buffer. If the last frame in the buffer is not wholly
			 * contained by it, then that frame's start is pointed by
			 * the next_frame member of the Stream structure. This
			 * common situation occurs when mad_frame_decode() fails,
			 * sets the stream error code to MAD_ERROR_BUFLEN, and
			 * sets the next_frame pointer to a non NULL value. (See
			 * also the comment marked {4} bellow.)
			 *
			 * When this occurs, the remaining unused bytes must be
			 * put back at the beginning of the buffer and taken in
			 * account before refilling the buffer. This means that
			 * the input buffer must be large enough to hold a whole
			 * frame at the highest observable bit-rate (currently 448
			 * kb/s). XXX=XXX Is 2016 bytes the size of the largest
			 * frame? (448000*(1152/32000))/8
			 */
			if(Stream.next_frame!=NULL)
			{
				Remaining=Stream.bufend-Stream.next_frame;
				memmove(InputBuffer,Stream.next_frame,Remaining);
				ReadStart=InputBuffer+Remaining;
				ReadSize=INPUT_BUFFER_SIZE-Remaining;
			}
			else {
				ReadSize=INPUT_BUFFER_SIZE;
					ReadStart=InputBuffer;
					Remaining=0;
			}

			/* Fill-in the buffer. If an error occurs print a message
			 * and leave the decoding loop. If the end of stream is
			 * reached we also leave the loop but the return status is
			 * left untouched.
			 */
			buffering = 1;
			ReadSize=BstdRead(ReadStart,1,ReadSize,BstdFile);
			if(ReadSize<=0)
			{
				if(EndOfFile(InputFp))
					printf("%s: end of input stream\n",ProgName);
				else
					printf("SOME FILE ERROR\n");
				break;
			}
			buffering = 0;

			/* {3} When decoding the last frame of a file, it must be
			 * followed by MAD_BUFFER_GUARD zero bytes if one wants to
			 * decode that last frame. When the end of file is
			 * detected we append that quantity of bytes at the end of
			 * the available data. Note that the buffer can't overflow
			 * as the guard size was allocated but not used the the
			 * buffer management code. (See also the comment marked
			 * {1}.)
			 *
			 * In a message to the mad-dev mailing list on May 29th,
			 * 2001, Rob Leslie explains the guard zone as follows:
			 *
			 *    "The reason for MAD_BUFFER_GUARD has to do with the
			 *    way decoding is performed. In Layer III, Huffman
			 *    decoding may inadvertently read a few bytes beyond
			 *    the end of the buffer in the case of certain invalid
			 *    input. This is not detected until after the fact. To
			 *    prevent this from causing problems, and also to
			 *    ensure the next frame's main_data_begin pointer is
			 *    always accessible, MAD requires MAD_BUFFER_GUARD
			 *    (currently 8) bytes to be present in the buffer past
			 *    the end of the current frame in order to decode the
			 *    frame."
			 */
			if(BstdFileEofP(BstdFile))
			{
				GuardPtr=ReadStart+ReadSize;
				memset(GuardPtr,0,MAD_BUFFER_GUARD);
				ReadSize+=MAD_BUFFER_GUARD;
			}

			/* Pipe the new buffer content to libmad's stream decoder
             * facility.
			 */
			mad_stream_buffer(&Stream,InputBuffer,ReadSize+Remaining);
			Stream.error= (mad_error)0;
		}




		/* Decode the next MPEG frame. The streams is read from the
		 * buffer, its constituents are break down and stored the the
		 * Frame structure, ready for examination/alteration or PCM
		 * synthesis. Decoding options are carried in the Frame
		 * structure from the Stream structure.
		 *
		 * Error handling: mad_frame_decode() returns a non zero value
		 * when an error occurs. The error condition can be checked in
		 * the error member of the Stream structure. A mad error is
		 * recoverable or fatal, the error status is checked with the
		 * MAD_RECOVERABLE macro.
		 *
		 * {4} When a fatal error is encountered all decoding
		 * activities shall be stopped, except when a MAD_ERROR_BUFLEN
		 * is signaled. This condition means that the
		 * mad_frame_decode() function needs more input to complete
		 * its work. One shall refill the buffer and repeat the
		 * mad_frame_decode() call. Some bytes may be left unused at
		 * the end of the buffer if those bytes forms an incomplete
		 * frame. Before refilling, the remaining bytes must be moved
		 * to the beginning of the buffer and used for input for the
		 * next mad_frame_decode() invocation. (See the comments
		 * marked {2} earlier for more details.)
		 *
		 * Recoverable errors are caused by malformed bit-streams, in
		 * this case one can call again mad_frame_decode() in order to
		 * skip the faulty part and re-sync to the next frame.
		 */
		if(mad_frame_decode(&Frame,&Stream))
		{
			if(MAD_RECOVERABLE(Stream.error))
			{
				/* Do not print a message if the error is a loss of
				 * synchronization and this loss is due to the end of
				 * stream guard bytes. (See the comments marked {3}
				 * supra for more informations about guard bytes.)
				 */
				if(Stream.error!=MAD_ERROR_LOSTSYNC ||
				   Stream.this_frame!=GuardPtr)
				{
					printf("%s: recoverable frame level error (%s)\n",
							ProgName,MadErrorString(&Stream));
				}
				continue;
			}
			else
				if(Stream.error==MAD_ERROR_BUFLEN)
					continue;
				else
				{
					printf("%s: unrecoverable frame level error (%s).\n",
							ProgName,MadErrorString(&Stream));
					Status=1;
					break;
				}
		}

		/* The characteristics of the stream's first frame is printed
		 * on stderr. The first frame is representative of the entire
		 * stream.
		 */
		if(FrameCount==0)
		{
			if(PrintFrameInfo(&Frame.header))
			{
				Status=1;
				break;
			}
		}

		/* Accounting. The computed frame duration is in the frame
		 * header structure. It is expressed as a fixed point number
		 * whole data type is mad_timer_t. It is different from the
		 * samples fixed point format and unlike it, it can't directly
		 * be added or subtracted. The timer module provides several
		 * functions to operate on such numbers. Be careful there, as
		 * some functions of libmad's timer module receive some of
		 * their mad_timer_t arguments by value!
		 */
		FrameCount++;
		mad_timer_add(&Timer,Frame.header.duration);

		/* Between the frame decoding and samples synthesis we can
		 * perform some operations on the audio data. We do this only
		 * if some processing was required. Detailed explanations are
		 * given in the ApplyFilter() function.
		 */

		/* Once decoded the frame is synthesized to PCM samples. No errors
		 * are reported by mad_synth_frame();
		 */
		mad_synth_frame(&Synth,&Frame);

		/* TIME TO RESAMPLE */
		if ( ( (int)Synth.pcm.length > 0) )
		 {
			 oldRate = in_type.rate;
			 in_type.rate = Synth.pcm.samplerate;
			 sampleSize = 0;
			 totalStreams++;
			 for(i=0;i<Synth.pcm.length;i++) //Store in sample buffer
			 {
				Sample=audio_linear_dither(16, Synth.pcm.samples[0][i],&left_dither);
				sampleBuffer[2*i] = Sample;
				if (Synth.pcm.channels == 2)
				{
					Sample=audio_linear_dither(16, Synth.pcm.samples[1][i],&right_dither);
					sampleBuffer[(2*i)+1] = Sample;
				}
				else if (Synth.pcm.channels == 1)
				{
					sampleBuffer[(2*i)+1] = Sample;
				}
			 }

			 if (oldRate != in_type.rate)
			 {
				 if (resamplerInMemory)
					 resampler->term (state);
				 resamplerInMemory = 1;
				 resampler = ls_get_resampler (in_type, out_type, LS_BEST);
				 if (!resampler)
				 {
					 printf ("failed to find resampler\n");
					 return -1;
				 }
				 state = resampler->init (in_type, out_type);
			 }
			 sampleSize = ls_resampled_size (in_type, out_type, Synth.pcm.length*2*2);
			 resampler->resample (state,sampleBuffer,resampleBuffer, Synth.pcm.length*2*2, sampleSize);


			 /*-----------------------RESAMPLING DONE (FILL THE BUFFER)------------------------*/
			 sampleSize = sampleSize/4;
			 for(i=0;(unsigned int)i<sampleSize;i++)
			 {
				 sBufferList[bufferSwap][2*bufferIndex] = resampleBuffer[2*i];
				 sBufferList[bufferSwap][(2*bufferIndex)+1] = resampleBuffer[(2*i)+1];
				 
				 bufferIndex++;
				 readSamples++;
				 
				 if (preBuffer == 1)
				 {
					 if (bufferIndex == PLAYBUFFER/4)  //One Buffer is full	
					 {
						 if (bufferSwap == 1) //Both buffers are now full
						 {
							 bufferIndex = 0;
							 preBuffer = 2;
							 bufferSwap++; bufferSwap = (bufferSwap % 2);
							 SjPCM_Clearbuff();
							 WakeupThread(pid);
							 SleepThread();
						 }
						 else
						 {
							 bufferSwap++; bufferSwap = (bufferSwap % 2);
							 bufferIndex = 0;
						 }
					 }
				 }
				 else if (preBuffer == 2)
				 {
					 {
						 if (bufferIndex == PLAYBUFFER/4)
						 {
							 bufferIndex = 0;
							 SleepThread();
						 }
					 }
				 }
			 }
		 }

		{
			static int i = 0;
			if (i++ % 64 == 0) // update 1/64th of the time
				cb(); 
		}

	}while(1); //The song has been buffered

	if ( (preBuffer == 1) && (breakOut == 0) ) //No need to stream (small file)
	{
		bufferIndex = 0;
		bufferSwap++; bufferSwap = (bufferSwap % 2);
		WakeupThread(pid);
	}
	/* The input file was completely read; the memory allocated by our
	 * reading module must be reclaimed.
	 */

	/* Mad is no longer used, the structures that were initialized must
     * now be cleared.
	 */
	mad_synth_finish(&Synth);
	mad_frame_finish(&Frame);
	mad_stream_finish(&Stream);

	/* If the output buffer is not empty and no error occurred during
     * the last write, then flush it.
	 */
	/* Accounting report if no error occurred. */
	if(!Status)
	{
		char	Buffer[80];

		/* The duration timer is converted to a human readable string
		 * with the versatile, but still constrained mad_timer_string()
		 * function, in a fashion not unlike strftime(). The main
		 * difference is that the timer is broken into several
		 * values according some of it's arguments. The units and
		 * fracunits arguments specify the intended conversion to be
		 * executed.
		 *
		 * The conversion unit (MAD_UNIT_MINUTES in our example) also
		 * specify the order and kind of conversion specifications
		 * that can be used in the format string.
		 *
		 * It is best to examine libmad's timer.c source-code for details
		 * of the available units, fraction of units, their meanings,
		 * the format arguments, etc.
		 */
		mad_timer_string(Timer,Buffer,"%lu:%02lu.%03u",
						 MAD_UNITS_MINUTES,MAD_UNITS_MILLISECONDS,0);
		printf("%s: %lu frames decoded (%s).\n",
				ProgName,FrameCount,Buffer);
	}
	preBuffer = 3;
	if (!breakOut)
		SleepThread();
	DeleteThread(pid);
	SjPCM_Clearbuff();
	rfree(sBuffer2);
	rfree(sampleBuffer);
	rfree(sBuffer1);
	rfree(resampleBuffer);
	BstdFileDestroy(BstdFile);
	resampler->term (state);

	wait_vsync();
	/*SjPCM_Quit();
	SjPCM_Init(0);*/
	SjPCM_Clearbuff();
	/*SjPCM_Setvol(volume);
	SjPCM_Play();*/

	/* That's the end of the world (in the H. G. Wells way). */
	return(Status);
}


/****************************************************************************
 * Print human readable informations about an audio MPEG frame.				*
 ****************************************************************************/
int PrintFrameInfo(struct mad_header *Header)
{
	const char	*Layer,
				*Mode,
				*Emphasis;
	// char		info[500]; moved to the uppest part of the file

	/* Convert the layer number to it's printed representation. */
	switch(Header->layer)
	{
		case MAD_LAYER_I:
			Layer="I";
			break;
		case MAD_LAYER_II:
			Layer="II";
			break;
		case MAD_LAYER_III:
			Layer="III";
			break;
		default:
			Layer="(unexpected layer value)";
			break;
	}

	/* Convert the audio mode to it's printed representation. */
	switch(Header->mode)
	{
		case MAD_MODE_SINGLE_CHANNEL:
			Mode="single channel";
			break;
		case MAD_MODE_DUAL_CHANNEL:
			Mode="dual channel";
			break;
		case MAD_MODE_JOINT_STEREO:
			Mode="joint stereo";
			break;
		case MAD_MODE_STEREO:
			Mode="normal LR stereo";
			break;
		default:
			Mode="(unexpected mode value)";
			break;
	}

	/* Convert the emphasis to it's printed representation. Note that
	 * the MAD_EMPHASIS_RESERVED enumeration value appeared in libmad
	 * version 0.15.0b.
	 */
	switch(Header->emphasis)
	{
		case MAD_EMPHASIS_NONE:
			Emphasis="none";
			break;
		case MAD_EMPHASIS_50_15_US:
			Emphasis="50/15 us";
			break;
		case MAD_EMPHASIS_CCITT_J_17:
			Emphasis="CCITT J.17";
			break;
#if (MAD_VERSION_MAJOR>=1) || \
	((MAD_VERSION_MAJOR==0) && (MAD_VERSION_MINOR>=15))
		case MAD_EMPHASIS_RESERVED:
			Emphasis="reserved(!)";
			break;
#endif
		default:
			Emphasis="(unexpected emphasis value)";
			break;
	}

	printf("%s: %lu kb/s audio MPEG layer %s stream %s CRC, "
			"%s with %s emphasis at %d Hz sample rate\n",
			ProgName,Header->bitrate,Layer,
			Header->flags&MAD_FLAG_PROTECTION?"with":"without",
			Mode,Emphasis,Header->samplerate);

			  
//	int textX = DrawFrame(50, 30, SCREEN_WIDTH-50, SCREEN_HEIGHT-30, gDrv);
//	ParseTitle(currentTitle, (SCREEN_WIDTH-50-textX)-(50+textX) );
//	PrintHeaderCenter(currentTitle, 50+textX, 80, SCREEN_WIDTH-50-textX, 0, 0, 0, gDrv);
	/*originals
	sprintf(info, "Bitrate: %lu\nLayer: %s stream\nCRC Protection: %s\nEmphasis: %s\n"\
				   "Audio Mode: %s\nSample Rate: %d Hz\n\n%s\n\n\n\n%s",
				   Header->bitrate, Layer, Header->flags&MAD_FLAG_PROTECTION?"Yes":"No",
				   Emphasis, Mode, Header->samplerate,repeatinfo,volumeinfo);
	sprintf(infodata,"Bitrate: %lu\nLayer: %s stream\nCRC Protection: %s\nEmphasis: %s\n"\
				   "Audio Mode: %s\nSample Rate: %d Hz",
				   Header->bitrate, Layer, Header->flags&MAD_FLAG_PROTECTION?"Yes":"No",
				   Emphasis, Mode, Header->samplerate);
//	PrintHeader(info, 50+textX, 180, SCREEN_WIDTH-50-textX, 0, 0, 110, gDrv);*/
	sprintf(info, "Bitrate:\nLayer:\nCRC Protection:\nEmphasis:\n"\
				   "Audio Mode:\nSample Rate:\n\n%s\n\n\n\n%s",repeatinfo,volumeinfo);
	sprintf(infodata,"         %lu KB/s\n       %s stream\n                %s\n          %s\n"\
				   "            %s\n             %d Hz",
				   (Header->bitrate)/1000, Layer, Header->flags&MAD_FLAG_PROTECTION?"Yes":"No",
				   Emphasis, Mode, Header->samplerate);
//	PrintHeader(info, 50+textX, 180, SCREEN_WIDTH-50-textX, 0, 0, 110, gDrv);
//	PrintHeader(infodata, 50+textX, 180, SCREEN_WIDTH-50-textX,60,60,60, gDrv);
//	RenderScreen(gDrv);
	return 0;
}

int getvolume()
{
	return volume;
}
int setvolume(int newvolume)
{
	volume = newvolume;
	SjPCM_Setvol(volume);
	return 0;
}

