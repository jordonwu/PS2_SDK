Independent SjPCM 2.2 (iSjPCM)
===========================================================

This is an modification of SjPCM 2.1 which does not require
LIBSD nor freesd, which means its a standalone irx module
without any spu2 module dependencies.

It requires the ps2sdk source to build.

EE:
Nothing changed, just "ported" over to ps2sdk.

IOP:
iSjPCM is now able to resample the sound buffer in case 
the sample rate is not 48Khz (IOP mem cost is 3840 bytes)

Resampling is made based on the sample size passed to 
the SjPCM_Enqueue(..) function (EE side). Conversion 
algo is very basic and only works with the following 
sample size :
- 960 for 48Khz (PAL) > no resampling in this case
- 480 for 24Khz (PAL)
- 320 for 16Khz (PAL)
- 240 for 12Khz (PAL)
- 120 for  8Khz (PAL)
		
- 800 for 48Khz (NTSC) > no resampling in this case		
- 400 for 24Khz (NTSC)
- 266 for 16Khz (NTSC) 
- 200 for 12Khz (NTSC)
- 100 for  8Khz (NTSC)

other values didnt' tested, so don't try it won't work!

All spu2 code is based on freesd source and is #ifdef'ed
to only contain the parts of each function required by
iSjPCM, if you compile without -DISJPCM it will include
all the spu2 code, in case you want to use spu2.c for 
something else.

You can also compile without -DNOPRINT, this will cause
iSjPCM to print info to the console (like the original 
SjPCM did).

All the optimizations were done to keep the size
of the irx to a minimum.

You can also build with -DLIBSD and it will compile
like the original SjPCM which depends on LIBSD/freesd

You can reduce the size of iSjPCM further by removing
the functions you don't need for your application. Like
SjPCM_Pause/Quit/etc.

I hope this releases will helpe some with sound coding :)

Lukasz Bruun (26th August 2004)
