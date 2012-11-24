/*
    ----------------------------------------------------------------------
    sjpcm_irx.c - SjPCM IOP-side code. (c) Nick Van Veen (aka Sjeep), 2002
    ---------------------------------------------------------------------
	
	Modifications by evilo, 2005
	Added some basic resampling function
	Remove dependencies with freesd.h
	
	Modifications by Lukasz Bruun, 2004
	Now it doesn't require LIBSD :)

    	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#include "irx_imports.h"

#if defined(LIBSD) && defined(ISJPCM)
	#error You must either define LIBSD or ISJPCM, not both at once.
#endif

#ifndef LIBSD
	#include "spu2.h"
#endif

#define MODNAME "iSjPCM"
#define M_PRINTF(format, args...)	printf(MODNAME ": " format, ## args)

#define BLOCKSIZE 960
static unsigned int numblocks = 20;

// LIBSD defines
#define SD_CORE_1		1
#define SD_INIT_COLD		0


#ifdef LIBSD
#define SdInit			sceSdInit
#define SdSetTransCallback	sceSdSetTransCallback
#define SdSetParam		sceSdSetParam
#define SdSetCoreAttr		sceSdSetCoreAttr
#define SdBlockTrans		sceSdBlockTrans
#define SdBlockTransStatus	sceSdBlockTransStatus
#endif

////////////////

#define	SJPCM_IRX	0xB0110C5
#define SJPCM_PUTS	0x01
#define	SJPCM_INIT	0x02
#define SJPCM_PLAY	0x03
#define SJPCM_PAUSE	0x04
#define SJPCM_SETVOL	0x05
#define SJPCM_ENQUEUE	0x06
#define SJPCM_CLEARBUFF	0x07
#define SJPCM_QUIT	0x08
#define SJPCM_GETAVAIL  0x09
#define SJPCM_GETBUFFD  0x10
#define SJPCM_SETNUMBLOCKS    0x11
#define SJPCM_SETTHRESHOLD    0x12

#define SJPCM_CALLBACK                0x12

#define TH_C		0x02000000


#define malloc(size) (AllocSysMemory(0,size,NULL)) 


SifRpcDataQueue_t qd;
SifRpcServerData_t Sd0;

void SjPCM_Thread(void* param);
void SjPCM_PlayThread(void* param);
static int SjPCM_TransCallback(void* param);

void* SjPCM_rpc_server(unsigned int fno, void *data, int size);
void* SjPCM_Puts(char* s);
void* SjPCM_Init(unsigned int* sbuff);
void* SjPCM_Enqueue(unsigned int* sbuff);
void* SjPCM_Play();
void* SjPCM_Pause();
void* SjPCM_Setvol(unsigned int* sbuff);
void* SjPCM_Clearbuff();
void* SjPCM_Available(unsigned int* sbuff);
void* SjPCM_Buffered(unsigned int* sbuff);
void* SjPCM_SetNumBlocks(unsigned int* sbuff);
void* SjPCM_SetThreshold(unsigned int* sbuff);
void* SjPCM_Quit();

extern void wmemcpy(void *dest, void *src, int numwords);



static unsigned int buffer[0x80];

int memoryAllocated = 0;

char *pcmbufl = NULL;
char *pcmbufr = NULL;
char *spubuf = NULL;

char *tempPCMbufl = NULL;
char *tempPCMbufr = NULL;

int readpos = 0;
int writepos = 0;

int volume = 0x3fff;

int transfer_sema = 0;
int play_tid = 0;

int intr_state;

int SyncFlag;

unsigned int threshold = 10;
char cmdData[16];

int _start ()
{
  iop_thread_t param;
  int th;

  FlushDcache();

  CpuEnableIntr(0);
//  EnableIntr(36);	// Enables SPU DMA (channel 0) interrupt.
  EnableIntr(40);	// Enables SPU DMA (channel 1) interrupt.
//  EnableIntr(9);	// Enables SPU IRQ interrupt.

  param.attr         = TH_C;
  param.thread       = SjPCM_Thread;
  param.priority     = 40;
  param.stacksize    = 0x800;
  param.option       = 0;
  th = CreateThread(&param);
  if (th > 0) {
  	StartThread(th,0);
	return 0;
  }
  else return 1;

}

void SjPCM_Thread(void* param)
{
	#ifndef NOPRINT
		printf("iSjPCM v2.2 - by Sjeep, Lukasz Bruun & Evilo \n");
	#endif

  #ifndef NOPRINT
	M_PRINTF("RPC Initialize\n");
  #endif

  SifInitRpc(0);

  SifSetRpcQueue(&qd, GetThreadId());
  SifRegisterRpc(&Sd0, SJPCM_IRX, (void *)SjPCM_rpc_server,(void *) &buffer[0],0,0,&qd);
  SifRpcLoop(&qd);
}

void* SjPCM_rpc_server(unsigned int fno, void *data, int size)
{

	switch(fno) {
		case SJPCM_INIT:
			return SjPCM_Init((unsigned*)data);
		case SJPCM_PUTS:
			return SjPCM_Puts((char*)data);
		case SJPCM_ENQUEUE:
			return SjPCM_Enqueue((unsigned*)data);
		case SJPCM_PLAY:
			return SjPCM_Play();
		case SJPCM_PAUSE:
			return SjPCM_Pause();
		case SJPCM_SETVOL:
			return SjPCM_Setvol((unsigned*)data);
		case SJPCM_CLEARBUFF:
			return SjPCM_Clearbuff();
		case SJPCM_QUIT:
			return SjPCM_Quit();
		case SJPCM_GETAVAIL:
			return SjPCM_Available((unsigned*)data);
		case SJPCM_GETBUFFD:
			return SjPCM_Buffered((unsigned*)data);
		case SJPCM_SETNUMBLOCKS:
			return SjPCM_SetNumBlocks((unsigned*)data);
		case SJPCM_SETTHRESHOLD:
			return SjPCM_SetThreshold((unsigned*)data);
	}

	return NULL;
}

void* SjPCM_Clearbuff()
{
	CpuSuspendIntr(&intr_state);

	memset(spubuf,0,0x800);
	memset(pcmbufl,0,BLOCKSIZE*2*numblocks);
	memset(pcmbufr,0,BLOCKSIZE*2*numblocks);
	memset(tempPCMbufl,0,BLOCKSIZE*2);
	memset(tempPCMbufr,0,BLOCKSIZE*2);

	CpuResumeIntr(intr_state);
	
	return NULL;
}

void* SjPCM_Play()
{
	SdSetParam(SD_CORE_1|SD_PARAM_BVOLL,volume);
	SdSetParam(SD_CORE_1|SD_PARAM_BVOLR,volume);

	return NULL;
}

void* SjPCM_Pause()
{
	SdSetParam(SD_CORE_1|SD_PARAM_BVOLL,0);
	SdSetParam(SD_CORE_1|SD_PARAM_BVOLR,0);

	return NULL;
}

void* SjPCM_Setvol(unsigned int* sbuff)
{
	volume = sbuff[5];

	SdSetParam(SD_CORE_1|SD_PARAM_BVOLL,volume);
	SdSetParam(SD_CORE_1|SD_PARAM_BVOLR,volume);

	return NULL;
}

void* SjPCM_Puts(char* s)
{
	#ifndef NOPRINT
	M_PRINTF("%s",s);
	#endif
	return NULL;
}

void* SjPCM_Init(unsigned int* sbuff)
{
	iop_sema_t sema;
	iop_thread_t play_thread;

	SyncFlag = sbuff[0];		

	sema.attr = 0; // SA_THFIFO;
	sema.initial = 0;
	sema.max = 1;
	transfer_sema= CreateSema(&sema);
	
	
	if(transfer_sema <= 0) {
		#ifndef NOPRINT
		M_PRINTF("Failed to create semaphore!\n");
		#endif
		ExitDeleteThread();
	}

	// Allocate memory
	if(!memoryAllocated)
	{
		pcmbufl = malloc(BLOCKSIZE*2*numblocks);
		tempPCMbufl = malloc(BLOCKSIZE*2);
		if ((pcmbufl == NULL) || (tempPCMbufl == NULL)) {
			#ifndef NOPRINT
			M_PRINTF("Failed to allocate memory for sound buffer!\n");
			#endif
			ExitDeleteThread();
		}
		pcmbufr = malloc(BLOCKSIZE*2*numblocks);
		tempPCMbufr = malloc(BLOCKSIZE*2);
		if((pcmbufr == NULL) || (tempPCMbufr == NULL)){
			#ifndef NOPRINT
			M_PRINTF("Failed to allocate memory for sound buffer!\n");
			#endif
			ExitDeleteThread();
		}
		spubuf = malloc(0x800);
		if(spubuf == NULL) {
			#ifndef NOPRINT
			M_PRINTF("Failed to allocate memory for sound buffer!\n");
			#endif
			ExitDeleteThread();
		}

		#ifndef NOPRINT
			M_PRINTF("Memory Allocated. %d bytes left.\n", QueryTotalFreeMemSize());
		#endif

		memoryAllocated = 1;
	}

	memset(pcmbufl,0,BLOCKSIZE*2*numblocks);
	memset(pcmbufr,0,BLOCKSIZE*2*numblocks);
	memset(tempPCMbufl,0,BLOCKSIZE*2);
	memset(tempPCMbufr,0,BLOCKSIZE*2);
	memset(spubuf,0,0x800);

	#ifndef NOPRINT
		M_PRINTF("Sound buffers cleared\n");
	#endif

	// Initialise SPU
	SdInit(SD_INIT_COLD);
	
	#ifndef NOPRINT
		M_PRINTF("SPU2 initialised!\n");
	#endif

	#ifndef NOPRINT
		SdSetCoreAttr(SD_CORE_1|SD_CORE_NOISE_CLK,0);
	#endif
	
	SdSetParam(SD_CORE_1|SD_PARAM_MVOLL,0x3fff);
	SdSetParam(SD_CORE_1|SD_PARAM_MVOLR,0x3fff);
	
	#ifndef NOPRINT
		SdSetParam(SD_CORE_1|SD_PARAM_BVOLL,volume);
		SdSetParam(SD_CORE_1|SD_PARAM_BVOLR,volume);
	#else
		SjPCM_Play();
	#endif

	SdSetTransCallback(1, (void *)SjPCM_TransCallback);

	// Start audio streaming
	SdBlockTrans(1,SD_BLOCK_TRANS_LOOP,spubuf, 0x800, 0);

	#ifndef NOPRINT
		M_PRINTF("Setting up playing thread\n");
	#endif

	// Start playing thread
	play_thread.attr         = TH_C;
  	play_thread.thread       = SjPCM_PlayThread;
  	play_thread.priority 	 = 39;
  	play_thread.stacksize    = 0x800;
  	play_thread.option       = 0;
  	play_tid = CreateThread(&play_thread);
	if (play_tid > 0) StartThread(play_tid,0);
	else {
		#ifndef NOPRINT
		M_PRINTF("SjPCM: Failed to start playing thread!\n");
		#endif
		ExitDeleteThread();
	}

	// Return data to temporary buff
	sbuff[1] = (unsigned)tempPCMbufl;
	sbuff[2] = (unsigned)tempPCMbufr;
	sbuff[3] = writepos;

	#ifndef NOPRINT
		M_PRINTF("Entering playing thread.\n");
	#endif

	return sbuff;
}

void SjPCM_PlayThread(void* param)
{
	int which;
	
	while(1) {

		WaitSema(transfer_sema);

		// Interrupts are suspended, instead of using semaphores.
		CpuSuspendIntr(&intr_state);

		which = 1 - (SdBlockTransStatus(1, 0 )>>24);

		wmemcpy(spubuf+(1024*which),pcmbufl+readpos,512);	// left
		wmemcpy(spubuf+(1024*which)+512,pcmbufr+readpos,512);	// right

		readpos += 512;
		if(readpos >= (BLOCKSIZE*2*numblocks)) readpos = 0;
		
		{
			unsigned int rp = readpos, wp = writepos;
			if (wp<rp) wp+=BLOCKSIZE*2*numblocks;
			if ((wp-rp) < (threshold*BLOCKSIZE)) {
				sceSifSendCmd(SJPCM_CALLBACK, cmdData, 16, NULL, NULL, 0);
			}
		}

		CpuResumeIntr(intr_state);

	}
}

// convert to 48Khz sample per frame
void up_samples(char *sleft, char *sright, char *dleft, char *dright,int dest_samples, int orig_samples) 
{
	int i;
	int ratio = (dest_samples/orig_samples);
	for(i=dest_samples;i--;)
	{
		*dleft++ = *sleft;
		*dleft++ = *(sleft+1);
		*dright++ = *sright;
		*dright++ = *(sright+1);
		if(!(i%ratio) && i){sleft++;sright++;sleft++;sright++;}
	}
	return;

} 

// this is a crappy upsampling code that will not work for all frequencies,
// but can be quite usefull for a lot of case !
void* SjPCM_Enqueue(unsigned int* sbuff)
{
	int sample_size;
	
	switch (sbuff[0])
	{
		// 48Khz PAL
		case 960:
		// 48Khz NTSC
		case 800: sample_size = sbuff[0];
			  break;
		
		// 24Khz PAL
		case 480: 
		// 16Khz PAL
		case 320:
		 // 12Khz PAL
		case 240:
		 //  8Khz PAL
		case 120: sample_size = 960;
			  break;
		
		// 24Khz NTSC
		case 400:
		 // 16Khz NTSC 
		case 266:
		 // 12Khz NTSC
		case 200:
		 //  8Khz NTSC
		case 100: sample_size = 800;
			  break;
		
		// other values not supported
		// for the moment
		default : sample_size = sbuff[0];
			  break;
	}
	
	

	// resample buffer if needed
	if (sbuff[0] != sample_size)
	{ 
	  up_samples(tempPCMbufl, tempPCMbufr,
	  	     pcmbufl+writepos, pcmbufr+writepos, 
	  	     sample_size, sbuff[0]);
	  
	  sbuff[0] = sample_size; // set expected size
	}
	// else just copy it
	else 
	{ 	
		wmemcpy(pcmbufl+writepos, tempPCMbufl, sample_size);	// left
		wmemcpy(pcmbufr+writepos, tempPCMbufr, sample_size);	// right
	
	}

	// compute next writepos
	writepos += sample_size * 2;
	if(writepos >= (BLOCKSIZE*2*numblocks)) writepos = 0;

	if(SyncFlag)
		if(writepos == (BLOCKSIZE*numblocks)) readpos = 0x2400;

	//sbuff[3] = writepos;
	// it's not used on the EE side anyway
	sbuff[3] = 0;
	
	// that's over doc' !
	return sbuff;
}

static int SjPCM_TransCallback(void* param)
{
	iSignalSema(transfer_sema);

	return 1;
}

void* SjPCM_Available(unsigned int* sbuff)
{
  unsigned int rp = readpos, wp = writepos;
  if (wp<rp) wp+=BLOCKSIZE*2*numblocks;
  sbuff[3] = (BLOCKSIZE*2*numblocks-(wp-rp))/4;
  return sbuff;
}

void* SjPCM_Buffered(unsigned int* sbuff)
{
  unsigned int rp = readpos, wp = writepos;
  if (wp<rp) wp+=BLOCKSIZE*2*numblocks;
  sbuff[3] = (wp-rp)/4;
  return sbuff;
}

void* SjPCM_SetNumBlocks(unsigned int* sbuff)
{
        if (pcmbufl) {
	        #ifndef NOPRINT
	        M_PRINTF("Failed to set number of buffers: SjPCM_Init already called!\n");
	        #endif
	        ExitDeleteThread();
        }
	
	numblocks = *sbuff;
	threshold = numblocks / 2;
	return sbuff;
}

void* SjPCM_SetThreshold(unsigned int* sbuff)
{
	threshold = *sbuff;
	return sbuff;
}

void* SjPCM_Quit(unsigned int* sbuff)
{
	SdSetTransCallback(1,NULL);
	SdBlockTrans(1,SD_BLOCK_TRANS_STOP,0,0,0);

	TerminateThread(play_tid);
	DeleteThread(play_tid);

	DeleteSema(transfer_sema);

	return sbuff;
}
