.SUFFIXES : .c .o
CC = /usr/bin/gcc
CFLAGS = -g
# CFLAGS = -g -DFTP
#LIBS = -lreadline -lcurses -lncurses -lftp
LIBS = -lreadline -lcurses -lncurses
INC = -L/usr/local/lib -I/usr/local/include -I/usr/include
NPLIBS = -lusb -lreadline -lc -lcurses -lncurses
RM = rm -rf
TAR = tar -cf
ZIP = gzip -9
FILES = pksh/Makefile pksh/README.npsh pksh/README.pksh  pksh/eeexec.c pksh/iopexec.c\
	pksh/pksh.c pksh/pksh.h pksh/linkproto_stub.c pksh/linkproto_core.c\
	pksh/reset.c pksh/naplink.c pksh/pl2301.h\
	pksh/pl2301.c pksh/npsh.h\
	pksh/npsh.c pksh/common.c pksh/common.h pksh/packet.c pksh/packet.h\
	pksh/naplink.h pksh/SAMPLE.npshrc\
    pksh/SAMPLE.pkshrc pksh/LICENSE pksh/rl_common.c pksh/rl_common.h\
	pksh/ps2fs.h pksh/netfsproto_core.c


BATCHIO_OBJ = common.o linkproto_stub.o linkproto_core.o batch.o
BATCH_OBJ = common.o linkproto_stub.o linkproto_core.o
PKSH_OBJ = rl_common.o common.o linkproto_stub.o linkproto_core.o pksh.o \
		netfsproto_core.o
NPSH_OBJ = rl_common.o common.o packet.o pl2301.o npsh.o naplink.o

# protective gear on
CFLAGS += -W -Wall -Wpointer-arith -Wcast-align
CFLAGS += -Wbad-function-cast -Wsign-compare
CFLAGS += -Waggregate-return -Wmissing-noreturn -Wnested-externs
CFLAGS += -Wchar-subscripts -Wformat-security

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $< $(INC)
.o:
	$(CC) $(CFLAGS) -o $@ -c $< $(INC)

all: pksh npsh
utils: eeexec iopexec reset dumpmem dumpreg viewmem gsexec

pksh: $(PKSH_OBJ)
	$(CC) -g $(PKSH_OBJ) -o $@ $(LIBS) $(INC)

eeexec: $(BATCHIO_OBJ) eeexec.o
	$(CC) $(BATCHIO_OBJ) eeexec.o -o $@

iopexec: $(BATCHIO_OBJ) iopexec.o
	$(CC) $(BATCHIO_OBJ) iopexec.o -o $@

reset: $(BATCH_OBJ) reset.o 
	$(CC) $(BATCH_OBJ) reset.o -o $@

gsexec: $(BATCHIO_OBJ) gsexec.o 
	$(CC) $(BATCHIO_OBJ) gsexec.o -o $@

dumpmem: $(BATCHIO_OBJ) dumpmem.o 
	$(CC) $(BATCHIO_OBJ) dumpmem.o -o $@

dumpreg: $(BATCHIO_OBJ) dumpreg.o 
	$(CC) $(BATCHIO_OBJ) dumpreg.o -o $@

viewmem: $(BATCHIO_OBJ) viewmem.o 
	$(CC) $(BATCHIO_OBJ) viewmem.o -o $@

naplink.o:
	$(CC) -c naplink.c $(INC)
packet.o:
	$(CC) -c packet.c $(INC)
pl2301.o:
	$(CC) -c pl2301.c $(INC)
npsh.o:
	$(CC) -c npsh.c $(INC)
npsh: $(NPSH_OBJ)
	$(CC) $(NPSH_OBJ) -o $@ $(INC) $(NPLIBS)

zip:
	@cd ..;\
	$(TAR) loadclients.tar ${FILES};\
    $(ZIP) loadclients.tar
	@echo "../loadclients.tar.gz"

clean:
	$(RM) *.o *.so

docs:
	doxygen doxy.conf
