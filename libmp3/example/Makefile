EE_BIN = ../bin/mp3play.elf
EE_OBJS = mp3play.o zlib-stub.o

#screenshot.o

EE_LIBS += -lmp3 -lfileXio -ldebug -lsample -lmad -lm -lcdvdfs -lkernel -lc \
	-lzlib -lid3tag -lc

# excluded: bar.o option.o

EE_CXXFLAGS += -fno-exceptions -fno-rtti

EE_INCS += -I$(LIBMP3)/ee/include  -I$(ID3TAG)  -I$(ZLIB)
EE_LDFLAGS += -L./LIB -L$(LIBMP3)/ee/lib/ -L$(LIBMAD)/ -L$(LIBMP3)/contrib/lib \
	-L$(ID3TAG) -L$(ZLIB)

all: $(EE_BIN)

clean:
	rm -f *.elf *.o *.a *.s $(EE_BIN)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
