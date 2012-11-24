# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2005.
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
#-----------------------------------------------------------------------
# flags : -DNOPRINT (disable printf)

IOP_OBJS_DIR = obj/
IOP_BIN_DIR = bin/
IOP_SRC_DIR = src/

BIN2S = $(PS2SDK)/bin/bin2s

IOP_BIN  = bin/isjpcm.irx
IOP_OBJS = obj/hw.o obj/sjpcm_irx.o obj/spu2.o obj/imports.o

IOP_CFLAGS  += -Wall -fno-builtin-printf -DISJPCM
IOP_INCS    += -I$(PS2SDK)/iop/include/ -I$(PS2SDK)/common/include/ -I./src/
IOP_LDFLAGS += -s

all: $(IOP_OBJS_DIR) $(IOP_BIN_DIR) $(IOP_BIN) builtin

builtin:
	$(BIN2S) $(IOP_BIN) $(IOP_BIN_DIR)isjpcmIrx.S isjpcmIrx

clean:
	rm -f -r $(IOP_OBJS_DIR) $(IOP_BIN_DIR)

install:
	mkdir -p $(PS2DEV)/isjpcm/bin
	cp $(IOP_BIN) $(PS2DEV)/isjpcm/bin

include $(PS2SDK)/Defs.make
include ./Rules.make
