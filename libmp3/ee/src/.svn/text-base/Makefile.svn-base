EE_LIB = ../lib/libmp3.a
EE_OBJS = libmp3.o bstdfile.o file.o directory.o mp3help.o sjpcm_rpc.o rmalloc.o

EE_CXXFLAGS += -fno-exceptions -fno-rtti
EE_LIBS += -lfileXio -lsample -lmad -lpad -lgs -lm -lcdvdfs -lc -lkernel \
	-lsyscall -lc 
EE_INCS += -I../include -I$(ID3TAG) 
EE_LDFLAGS += -L../../contrib/lib/ -L$(ID3TAG)

all: $(EE_LIB)

clean:
	rm -f *.elf *.o ../lib/*.a 

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
