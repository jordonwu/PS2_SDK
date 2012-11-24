/*
    -----------------------------------------------------------------------
    mp3help.c - PS2MP3. (c) Ryan Kegel, 2004
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

#include "string.h"
#include "libhdd.h"
#include "fileio.h"
#include "debug.h"
#include "sjpcm.h"
#include "sbv_patches.h"
#include "bstdfile.h"
#include "mad.h"
#include "rmalloc.h"
#include "mp3help.h"


#define SHRT_MAX ((short)(((unsigned short)(~0U))>>1))
#define SHRT_MIN (-SHRT_MAX-1)

const char *ProgName = "MP3 PLAY";

//Used to downsample 24bit to 16bit
struct audio_dither 
{
    mad_fixed_t error[3];
    mad_fixed_t random;
};


/****************************************************************************
 * This function will return specific error messaged for libmad.			*
 ****************************************************************************/
#if (MAD_VERSION_MAJOR>=1) || \
    ((MAD_VERSION_MAJOR==0) && \
     (((MAD_VERSION_MINOR==14) && \
       (MAD_VERSION_PATCH>=2)) || \
      (MAD_VERSION_MINOR>14)))
#define MadErrorString(x) mad_stream_errorstr(x)
#else
const char *MadErrorString(const struct mad_stream *Stream)
{
	switch(Stream->error)
	{
		/* Generic unrecoverable errors. */
		case MAD_ERROR_BUFLEN:
			return("input buffer too small (or EOF)");
		case MAD_ERROR_BUFPTR:
			return("invalid (null) buffer pointer");
		case MAD_ERROR_NOMEM:
			return("not enough memory");

		/* Frame header related unrecoverable errors. */
		case MAD_ERROR_LOSTSYNC:
			return("lost synchronization");
		case MAD_ERROR_BADLAYER:
			return("reserved header layer value");
		case MAD_ERROR_BADBITRATE:
			return("forbidden bitrate value");
		case MAD_ERROR_BADSAMPLERATE:
			return("reserved sample frequency value");
		case MAD_ERROR_BADEMPHASIS:
			return("reserved emphasis value");

		/* Recoverable errors */
		case MAD_ERROR_BADCRC:
			return("CRC check failed");
		case MAD_ERROR_BADBITALLOC:
			return("forbidden bit allocation value");
		case MAD_ERROR_BADSCALEFACTOR:
			return("bad scalefactor index");
		case MAD_ERROR_BADFRAMELEN:
			return("bad frame length");
		case MAD_ERROR_BADBIGVALUES:
			return("bad big_values count");
		case MAD_ERROR_BADBLOCKTYPE:
			return("reserved block_type");
		case MAD_ERROR_BADSCFSI:
			return("bad scalefactor selection info");
		case MAD_ERROR_BADDATAPTR:
			return("bad main_data_begin pointer");
		case MAD_ERROR_BADPART3LEN:
			return("bad audio data length");
		case MAD_ERROR_BADHUFFTABLE:
			return("bad Huffman table select");
		case MAD_ERROR_BADHUFFDATA:
			return("Huffman data overrun");
		case MAD_ERROR_BADSTEREO:
			return("incompatible block_type for JS");

		/* Unknown error. This switch may be out of sync with libmad's
		 * defined error codes.
		 */
		default:
			return("Unknown error code");
	}
}
#endif

/****************************************************************************
 * Generate a random number.												*
 ****************************************************************************/
inline unsigned long prng(unsigned long state)
{
    return (state * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;
}


/****************************************************************************
 * Function will dither a 24bit audio sample and return a 16bit sample.		*
 ****************************************************************************/
inline short audio_linear_dither(unsigned int bits, mad_fixed_t sample,
				struct audio_dither *dither)
{
  unsigned int scalebits;
  mad_fixed_t output, mask, random;

  enum {
    MIN = -MAD_F_ONE,
    MAX =  MAD_F_ONE - 1
  };

  /* noise shape */
  sample += dither->error[0] - dither->error[1] + dither->error[2];

  dither->error[2] = dither->error[1];
  dither->error[1] = dither->error[0] / 2;

  /* bias */
  output = sample + (1L << (MAD_F_FRACBITS + 1 - bits - 1));

  scalebits = MAD_F_FRACBITS + 1 - bits;
  mask = (1L << scalebits) - 1;

  /* dither */
  random  = prng(dither->random);
  output += (random & mask) - (dither->random & mask);

  dither->random = random;

  /* clip */
    if (output > MAX) 
	{
		output = MAX;
		if (sample > MAX)
			sample = MAX;
	}
    else if (output < MIN) 
	{
		output = MIN;
		if (sample < MIN)
			sample = MIN;
    }

  /* quantize */
  output &= ~mask;

  /* error feedback */
  dither->error[0] = sample - output;

  /* scale */
  return output >> scalebits;
}
	

/****************************************************************************
 * Converts a sample from libmad's fixed point number format to a signed	*
 * short (16 bits).		(Not Used)													*
 ****************************************************************************/
signed short MadFixedToSshort(mad_fixed_t Fixed)
{
	/* A fixed point number is formed of the following bit pattern:
	 *
	 * SWWWFFFFFFFFFFFFFFFFFFFFFFFFFFFF
	 * MSB                          LSB
	 * S ==> Sign (0 is positive, 1 is negative)
	 * W ==> Whole part bits
	 * F ==> Fractional part bits
	 *
	 * This pattern contains MAD_F_FRACBITS fractional bits, one
	 * should alway use this macro when working on the bits of a fixed
	 * point number. It is not guaranteed to be constant over the
	 * different platforms supported by libmad.
	 *
	 * The signed short value is formed, after clipping, by the least
	 * significant whole part bit, followed by the 15 most significant
	 * fractional part bits. Warning: this is a quick and dirty way to
	 * compute the 16-bit number, madplay includes much better
	 * algorithms.
	 */

	/* Clipping */
	if(Fixed>=MAD_F_ONE)
		return(SHRT_MAX);
	if(Fixed<=-MAD_F_ONE)
		return(-SHRT_MAX);

	/* Conversion. */
	Fixed=Fixed>>(MAD_F_FRACBITS-15);
	return((signed short)Fixed);
}

