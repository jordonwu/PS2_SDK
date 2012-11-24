/*
    -----------------------------------------------------------------------
    main.cpp - PS2MP3. (c) Ryan Kegel, 2004
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

/* PROBABLY WAY MORE INCLUDES THAN NECESSARY*/
#include "tamtypes.h"
#include <stdio.h>
#include <sifrpc.h>
#include <sifcmd.h>
#include "sys/stat.h"
#include "sys/fcntl.h"
#include "kernel.h"
#include "sifrpc.h"
#include "sif.h"
#include "stdarg.h"
#include "iopheap.h"
#include "sys/ioctl.h"
#include "fileXio_rpc.h"
#include "errno.h"
#include "string.h"
#include "fileio.h"
#include "debug.h"
#include "sjpcm.h"
#include "malloc.h"

#include "rmalloc.h"
#include "libmp3.h"
#include "id3tag.h"

/*
#include "bstdfile.h"
#include "directory.h" 
#include "file.h"
*/

int ret = 0;

//void ReturnToBrowser();

/* The IRX files to combine to the ELF file*/
char *FileExt_t[2] = {"mp3", "m3u"};

#define LOAD_MODULE(x) \
	ret = SifLoadModule("host:irx/" x, 0, NULL); \
	if (ret < 0) \
		{ printf("Error:\n\nModule " x " could not be loaded."); return -1; }
		

int loadModules()
{
	int ret = 0;

	SifLoadModule("rom0:LIBSD", 0, NULL);
	LOAD_MODULE("sjpcm.irx")
	
	return 1;
}

void mp3_callback(int *data) { // fixme: pass useful information 
	/* intended to read pad or update graphics, whatever */

	static int i = 0;
	printf (".");
}

int main(int argc, char **argv)
{
	char *filename = "host:contrib/test.mp3";
	struct id3_file *tag_file;
	struct id3_tag *tag;
	struct id3_frame *frame;

	if (argc > 1) {
		filename = argv[1];
	}

	SifInitRpc(0);
	
	if (loadModules() < 0)
		return -1;

	rmallocInit();  //Initialize memory system

	/*Setup up the playback device */
	SjPCM_Init(0);
	SjPCM_Clearbuff();
	SjPCM_Setvol(0x3fff);
	SjPCM_Play();

/*
	printf ("Reading ID3 from '%s'\n", filename);
  	tag_file = id3_file_open(filename, ID3_FILE_MODE_READONLY);
  	tag = id3_file_tag(tag_file);
	frame = id3_tag_findframe(tag, ID3_FRAME_TITLE, 0); // first frame
	id3_file_close(tag_file);
*/

	printf ("Playing MP3 '%s'\n", filename);
	PlayMP3(filename, (functionPointer) mp3_callback);

	SifInitRpc(0);
	return 0;
}

