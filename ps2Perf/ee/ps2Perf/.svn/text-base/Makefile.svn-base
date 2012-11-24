#  _____     ___ ____
#   ____|   |    ____|      PSX2 OpenSource Project
#  |     ___|   |____       (C)2003, Mavy & Bigboss ( mavy@ps2reality.net , 
#						      bigboss@ps2reality.net )
#  ---------------------------------------------------------------------------


EE_LIB = ../lib/libps2Perf.a
EE_OBJS = ps2Perf.o

all: $(EE_LIB)

install: $(EE_LIB)
	mkdir -p $(PS2DEV)/ps2Perf/lib
	cp $(EE_LIB) $(PS2DEV)/ps2Perf/lib
	mkdir -p $(PS2DEV)/ps2Perf/include
	cp ../../include/ps2Perf.h $(PS2DEV)/ps2Perf/include

clean:
	rm -f *.elf *.o *.a

include ../../Makefile.pref
include ../../Makefile.eeglobal
