#  _____     ___ ____
#   ____|   |    ____|      PSX2 OpenSource Project
#  |     ___|   |____       (C)2002, David Ryan ( Oobles@hotmail.com )
#  ------------------------------------------------------------------------

# Generated automatically from Makefile.in by configure.
#.SUFFIXES: .S .c .o .s .elf .irx

#  ------------------------------------------------------------------------
#  COMPILERS

IOPCC = iop-gcc
IOPAR = iop-ar
IOPLD = iop-ld
IOPAS = iop-as
EECC  = ee-gcc
EEAR  = ee-ar
EELD  = ee-gcc


#  ------------------------------------------------------------------------
#  DIRECTORY PATHS & FLAGS

ELF2IRX = $(PS2LIB)/iop/utils/elf2irx/elf2irx

EECFLAGS  = -mips3 -EL -nostartfiles 
EEINCLUDES = -I. -I$(PS2LIB)/common/include -I$(PS2LIB)/ee/include
EECOMPILE = $(EECC) $(EEINCLUDES) $(EECFLAGS) 
EELINK = $(EELD)  

# -g

IOPCFLAGS = -fomit-frame-pointer -Wall -G0 -EL -mcpu=r3000
IOPINCLUDES = -I. -I$(PS2LIB)/common/include -I$(PS2LIB)/iop/include
IOPCOMPILE = $(IOPCC) $(IOPINCLUDES) $(IOPCFLAGS)
IOPLINK = $(IOPLD) -G0 -nostdlib --warn-once --script mipsirx.x
# -dc 

INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA =${INSTALL} -m 644
mkinstalldirs = $(SHELL) $(top_builddir)/mkinstalldirs


#  ------------------------------------------------------------------------
#  PROJECTS TO BUILD

all: ps2debug.irx libiop_ps2debug.a


clean:
	rm *.irx
	rm *.o


#  ------------------------------------------------------------------------
#  PS2DEBUG.IRX BUILD INSTRUCTIONS

ps2debug_LDFLAGS = -L$(PS2LIB)/iop/lib
ps2debug_LDADD = -lkernel
ps2debug_OBJECTS = ps2debug.o ps2dexp.o


ps2debug.irx: $(ps2debug_OBJECTS) 
	$(IOPLINK) $(ps2debug_OBJECTS) $(ps2debug_LDFLAGS) $(ps2debug_LDADD) -o $@

ps2debug.o: ps2debug.c
	$(IOPCOMPILE) -o $@ -c $<

ps2dexp.o: ps2dexp.s
	$(IOPCOMPILE) -o $@ -c $<


#  ------------------------------------------------------------------------
#  LIBIOP_PS2DEBUG.A BUILD INSTRUCTIONS

libiop_ps2debug.a : iop_ps2debug.o
	$(IOPAR) cruv libiop_ps2debug.a iop_ps2debug.o

iop_ps2debug.o: ps2dimp.s
	$(IOPCOMPILE) -o $@ -c $<


