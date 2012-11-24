#  _____     ___ ____
#   ____|   |    ____|      PSX2 OpenSource Project
#  |     ___|   |____       (C)2003, Mavy & Bigboss ( mavy@ps2reality.net , 
#						      bigboss@ps2reality.net )
#  ---------------------------------------------------------------------------


EE_BIN = ps2PerfExample.elf
EE_OBJS = main.o 
EE_LIBS = -lps2Perf
EE_LDFLAGS = -L../lib

all: $(EE_BIN)
	cp -f ps2PerfExample.elf ../../bin/

clean:
	rm -f *.elf *.o *.a

include ../../Makefile.pref
include ../../Makefile.eeglobal
