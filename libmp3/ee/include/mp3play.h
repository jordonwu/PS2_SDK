#include "kernel.h"
#include "mad.h"

extern const char *ProgName;
struct audio_dither;

int MpegAudioDecoder(int InputFp);
int PrintFrameInfo(struct mad_header *Header);
