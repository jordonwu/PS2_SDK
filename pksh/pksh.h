/*
 * Copyright (c) Khaled Daham
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author(s) may not be used to endorse or promote products 
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/time.h>

/* readline headers */
#include <readline/readline.h>
#include <readline/history.h>

#include "linkproto_core.h"
#include "linkproto_stub.h"
#include "netfsproto_core.h"
#include "common.h"
#include "rl_common.h"

#define MAX_SIZE        8192
#define MAX_NO_PROMPT_COUNT 20
#define MAX_SEC_BLOCK   0
#define MAX_USEC_BLOCK  100000
#define LOG_F_CREAT  (O_WRONLY | O_CREAT | O_TRUNC)
#define LOG_F_MODE  (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define USEC 1000000
#define MAX_CLIENTS 10

/* variables */
int doloop = 1;
int update_prompt = 0;
int log_f_fd = 0;
int log_to_file = 0;
int VERBOSE = 0;
int DUMPSCREEN = 0;
int state = 0;

int pko_log_read(int);

void read_config(void);
int initialize_readline(void);
int pko_srv_read(int);
int dumpmem(char *file, unsigned int offset, unsigned int size);
int dumpregs(char *file, unsigned int regs);

int cli_cd(), cli_help(), cli_list(), cli_make();
int cli_pwd(), cli_quit(), cli_execee(), cli_execeiop();
int cli_reset(), cli_status(), cli_log(), cli_verbose();
int cli_setroot(), cli_debug(), cli_gsexec();
int cli_poweroff(), cli_exception(), cli_dumpmem(), cli_dumpregs();
int cli_vustop(), cli_vustart(), cli_writemem();
// ps2 net fs commands
int cli_mkdir(), cli_ps2format(), cli_ps2sync(), cli_rmdir();
int cli_ps2mount(), cli_rename(), cli_ps2umount(), cli_devlist();
int cli_copy();
// ftp commands
int cli_ftp(), cli_ftpget(), cli_ftpput(), cli_ftpdir(), cli_ftpdelete();
int cli_ftplist(), cli_ftpclose(), cli_ftpsite();

typedef struct {
    char *name;     /* User printable name of the function. */
    Function *func; /* Function to call to do the job. */
    char *doc;      /* Documentation for this function.  */
} COMMAND;


COMMAND * find_command(char *);

COMMAND commands[] = {
    { "?", cli_help, "? :: Synonym for `help'." },
    { "cd", cli_cd, "cd [dir] :: Change pksh directory to [dir]." },
    { "debug", cli_debug, "debug :: Show pksh debug messages. ( alt-d )" },
    { "dumpmem", cli_dumpmem, "dumpmem [file] [offset] [size] :: Dump [size] bytes of memory from [offset] to [file]." },
    { "dumpreg", cli_dumpregs, "dumpreg [file] [dma|intc|timer|gs|sif|fifo|gif|vif0|vif1|ipu|all] :: Dumps given registers to file." },
    { "exit", cli_quit, "exit :: Exits pksh ( alt-q )" },
    { "eeexec", cli_execee, "eeexec [file] [argn] ... :: Execute file on EE." },
    { "exception", cli_exception, "exception [screen|console] :: Exception dumps to [screen] or [console]." },
    { "execee", cli_execee, "execee [file] [argn] ... :: Execute file on EE." },
    { "execiop", cli_execeiop, "execiop [file] :: Execute file on IOP." },
    { "execgs", cli_gsexec, "execgs [file] :: Send display list to GS." },
    { "gsexec", cli_gsexec, "gsexec [file] :: Send display list to GS." },
    { "help", cli_help, "help :: Display this text." },
    { "iopexec", cli_execeiop, "iopexec [file] :: Execute file on IOP." },
    { "list", cli_list, "list [dir] :: List files in [dir]." },
    { "log", cli_log, "log [file] :: Log messages from PS2 to [file]."},
    { "ls", cli_list, "ls [dir] :: Synonym for list" },
    { "make", cli_make, "make [argn] ... :: Execute make [argn] ..." },
    { "poweroff", cli_poweroff, "poweroff :: Poweroff the PS2"},
    { "pwd", cli_pwd, "pwd :: Print the current working directory ( alt-p )" },
    { "quit", cli_quit, "quit :: Quit pksh ( alt-q )" },
    { "reset", cli_reset, "reset :: Resets ps2 side ( alt-r )" },
    { "setroot", cli_setroot, "setroot [dir] :: Sets [dir] to be root dir." },
    { "status", cli_status, "status :: Display some pksh information. ( alt-s )" },
    { "verbose", cli_verbose, "verbose :: Show verbose pksh messages. ( alt-v )" },
    { "vustart", cli_vustart, "vustart [0|1] :: Start vu0+vif0 or vu1+vif1." },
    { "vustop", cli_vustop, "vustop [0|1] :: Stop vu0+vif0 or vu1+vif1." },
    { "writemem", cli_writemem, "writemem [file] [offset] :: Writes the contents of the file to offset"},
    // PS2 NetFS commands
    { "cp", cli_copy, "copy :: cp [source] [destination]"},
    { "devlist", cli_devlist, "devlist ::" },
    { "format", cli_ps2format, "format ... "},
    { "mkdir", cli_mkdir, "mkdir [device]:[dir] ... :: remove dir [dir] on device [device] (default is hdd:) ..."},
    { "ps2mount", cli_ps2mount, "mount [device]:[dir] ... :: remove dir [dir] on device [device] (default is hdd:) ..."},
    { "rmdir", cli_rmdir, "rmdir [device]:[dir] ... :: remove dir [dir] on device [device] (default is hdd:) ..." },
    { "rm", cli_rename, "rename [devive]:[dir] :: rename ..." },
    { "ps2sync", cli_ps2sync, "sync :: Sync IO operation with ps2netfs" },
    { "ps2umount", cli_ps2umount, "umount [devive]:[dir] :: "},

    #ifdef FTP
    { "ftp", cli_ftp, "ftp init command"},
    { "dir", cli_ftpdir, "ftp dir command"},
    { "flist", cli_ftplist, "ftp list command"},
    { "put", cli_ftpput, "ftp put command"},
    { "get", cli_ftpget, "ftp get command"},
    { "dele", cli_ftpdelete, "ftp dele command"},
    { "close", cli_ftpclose, "ftp close command"},
    { "site", cli_ftpsite, "ftp site command"},
    #endif

    { (char *)NULL, (Function *)NULL, (char *)NULL }
};

