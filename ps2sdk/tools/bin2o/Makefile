# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
#
# $Id: Makefile 921 2005-03-20 13:45:03Z pixel $


TOOLS_OBJS_DIR = obj/
TOOLS_BIN_DIR = bin/
TOOLS_SRC_DIR = src/
TOOLS_INC_DIR = include/

TOOLS_OBJS = bin2o.o
TOOLS_OBJS := $(TOOLS_OBJS:%=$(TOOLS_OBJS_DIR)%)

TOOLS_BIN = $(TOOLS_BIN_DIR)bin2o

all: $(TOOLS_OBJS_DIR) $(TOOLS_BIN_DIR) $(TOOLS_BIN)

clean:
	rm -f -r $(TOOLS_OBJS_DIR) $(TOOLS_BIN_DIR)


include $(PS2SDKSRC)/Defs.make
include $(PS2SDKSRC)/tools/Rules.make
include $(PS2SDKSRC)/tools/Rules.release
