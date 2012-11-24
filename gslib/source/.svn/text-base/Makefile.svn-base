OBJS =  gsDriver.o \
	gsPipe.o \
	gsVram.o \
	gsFont.o \
	hw.o

all: $(OBJS)
	$(EE_AR) rcs $(LIBGS) $(OBJS)

clean:
	rm -f $(OBJS)

include ../Makefile.pref
include ../Makefile.global
