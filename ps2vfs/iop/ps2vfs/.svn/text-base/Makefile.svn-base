#  _____     ___ ____
#   ____|   |    ____|      PSX2 OpenSource Project
#  |     ___|   |____       (C)2002, David Ryan ( Oobles@hotmail.com )
#  ------------------------------------------------------------------------

IOP_BIN = ps2vfs.irx
IOP_OBJS = ps2vfs.o
IOP_LIBS = -liop_ps2ip

all: $(IOP_BIN)
	cp -f ps2vfs.irx ../bin/

clean:
	rm -f *.elf *.o *.a *.irx

include ../../Makefile.pref
include ../../Makefile.iopglobal
