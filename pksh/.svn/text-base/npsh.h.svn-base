/*
 * NapLink Linux Client v1.0.1 by AndrewK of Napalm
 *
 * Copyright (c) Andrew Kieschnick.
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/file.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/types.h>

/* readline headers */
#include <readline/readline.h>
#include <readline/history.h>

#define PORT 5900
#define MAX_PACKET 8192
#define LOG_F_CREAT  (O_WRONLY | O_CREAT | O_TRUNC)
#define LOG_F_MODE  (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#define PACKET_RESET      0x004d704e
#define PACKET_EXECPS2    0x014d704e
#define PACKET_EXECIOP    0x024d704e
#define PACKET_QUIT       0x034d704e
#define PACKET_BENCHMARK  0x044d704e

char * execee(char *);
char * execiop(char *);
void err_msg(char *);
int setup_con(char *, int);
int s_fd;

typedef struct {
    char *name;     /* User printable name of the function. */
    Function *func; /* Function to call to do the job. */
    char *doc;      /* Documentation for this function.  */
} COMMAND;

char ** fileman_completion(char *, int , int);
char * command_generator(char *, int);
void read_config(void);

int cli_cd(), cli_delete(), cli_help(), cli_list(), cli_make();
int cli_rename(), cli_pwd(), cli_quit(), cli_execee(), cli_execeiop();
int cli_reset(), cli_status(), cli_log(), cli_verbose();
int cli_debug(), cli_dump(), cli_benchmark();

COMMAND * find_command(char *);

COMMAND commands[] = {
    { "?", cli_help, "Synonym for `help'" },
    { "benchmark", cli_benchmark, "Small speed test." },
    { "cd", cli_cd, "Change to directory <dir>" },
    { "delete", cli_delete, "Delete <file>" },
    { "debug", cli_debug, "Show debug messages. ( alt-d )" },
    { "eeexec", cli_execee, "Execute file on EE" },
    { "execee", cli_execee, "Execute file on EE" },
    { "execiop", cli_execeiop, "Execute file on IOP" },
    { "help", cli_help, "Display this text" },
    { "iopexec", cli_execeiop, "Execute file on IOP" },
    { "list", cli_list, "List files in <dir>" },
    { "log", cli_log, "Log to <file>." },
    { "ls", cli_list, "Synonym for `list'" },
    { "make", cli_make, "will execute make <arg>" },
    { "pwd", cli_pwd, "Print the current working directory ( alt-p )" },
    { "quit", cli_quit, "Quit using Fileman ( alt-q )" },
    { "rename", cli_rename, "Rename <file> to <newname>" },
    { "reset", cli_reset, "Resets ps2 side ( alt-r )" },
    { "status", cli_status, "Displays open descriptors. ( alt-s )" },
    { "verbose", cli_verbose, "Show verbose messages. ( alt-v )" },
    { (char *)NULL, (Function *)NULL, (char *)NULL }
};

int doloop = 1;
int quit = 0;
int update_prompt = 0;
int log_f_fd = 0;
int log_to_file = 0;
int time1, time2, time_base = 0;
static char cmd[MAXPATHLEN];
struct timeval benchtime;
static int VERBOSE = 0;
static int DEBUG = 0;
static char pksh_history[MAXPATHLEN];
