#include <tamtypes.h>
#include <stdlib.h>
#include <sifcmd.h>
#include <kernel.h>
#include <sifrpc.h>
#include <libjpg.h>
#include <gsKit.h>
#include <dmaKit.h>
#include <stdio.h>

#include "jpegtest.h"

GSGLOBAL *gsGlobal;

void displayjpeg(jpgData *jpg) {
	GSTEXTURE tex;
	u8 *data;
	int i;

	data = malloc(jpg->width * jpg->height * (jpg->bpp/8));
	jpgReadImage(jpg, data);
	tex.Width =  jpg->width;
	tex.Height = jpg->height;
	tex.PSM = GS_PSM_CT24;
	tex.Vram = 0;
	tex.Mem = (u32*) data;
	gsKit_texture_upload(gsGlobal, &tex);
	jpgClose(jpg);
	free(data);

	for (i=0; i<120; i++) {
		gsKit_vsync();
	}
}

int main() {
	jpgData *jpg;

	SifInitRpc(0);
    
    printf("start\n");

	dmaKit_init(D_CTRL_RELE_ON, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	gsGlobal = gsKit_init_global(GS_MODE_NTSC);
	gsGlobal->PSM = GS_PSM_CT24;
	gsKit_init_screen(gsGlobal);


	printf("display raw jpeg\n");
	jpg = jpgOpenRAW(jpegtest, sizeof(jpegtest));
	if (jpg == NULL) {
		printf("error opening raw jpeg\n");
	} else {
		displayjpeg(jpg);
	}

	printf("display cdrom0:\\TEST.JPG;1\n");
	jpg = jpgOpen("cdrom0:\\TEST.JPG;1");
	if (jpg == NULL) {
		printf("error opening cdrom0:\\TEST.JPG;1\n");
	} else {
		displayjpeg(jpg);
	}

	printf("display host0:testorig.jpg\n");
	jpg = jpgOpen("host0:testorig.jpg");
	if (jpg == NULL) {
		printf("error opening host0:testorig.jpg\n");
	} else {
		displayjpeg(jpg);
	}

	printf("create screenshot file host0:screen.jpg\n");
	//ps2_screenshot_jpg("host0:screen.jpg", 0, 640, 480, GS_PSM_CT32);
    jpgScreenshot("host0:screen.jpg", gsGlobal->CurrentPointer,
                             gsGlobal->Width, gsGlobal->Height, gsGlobal->PSM);

	return 0;
}
