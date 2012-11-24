/*
 * NapLink Shell v1.0
 *
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

#include "npsh.h"
#include "common.h"
#include "rl_common.h"
#include "naplink.h"
#include "list.h"

static unsigned int timeout;
char *prompt = "npsh: ";
static unsigned int timedout = 0;

int
main(int argc, char* argv[])
{
    int ret, i;
    struct timeval time, start, last;
    fd_set rset, mset;
    VERBOSE = 0;
    printf("npsh v1.0\n");
    read_config();

    fcntl(0, F_SETFL, O_NONBLOCK);
    FD_ZERO(&mset);
    FD_SET(0, &mset);
    initialize_readline();
    if ( initialize_naplink() != 0 ) {
        rl_callback_handler_remove();
        return(1);
    }
    np_set_counter(0);
    naplink_check();
    naplink_process();
    gettimeofday(&start, NULL);
    while(doloop)
    {
        time.tv_sec = 0;
        time.tv_usec = 100;
        rset = mset;
        if ( !timedout ) {
            naplink_check();
            naplink_process();
            np_set_processing(0);
        }
        ret = select(0+1, &rset, NULL, NULL, &time);
        if ( ret < 0 )
        {
            perror("select");
        } else if (ret == 0) {
        } else {
            if ( timedout ) {
                printf("we timedout lets reinit\n");
                initialize_naplink();
                np_set_counter(0);
                naplink_check();
                naplink_process();
                timedout = 0;
                np_set_processing(0);
            }

            if (FD_ISSET(0, &rset))
            {
                rl_callback_read_char();
            }
        }
        if ( !timedout ) {
            for (i = 0; i < 10; i++)
            {
                naplink_process();
            }
        }
        if ( timeout > 0 ) {
            if (np_get_counter() > timeout) {
                naplink_close();
                np_set_counter(0);
                timedout = 1;
            }
        }
    }

    naplink_close();
    
    rl_callback_handler_remove();
    if ( log_to_file )
        if ((ret = close(log_f_fd)) == -1)
            perror("log_f_fd");
    if (strcmp(pksh_history, "") != 0 ) {
        if ( (ret = write_history(pksh_history)) != 0) 
            perror("write_history");
    }
    return(0);
}

/* 
 *      CLI commands
 */
static char clicom[1024];

int
execute_line(line)
    char *line;
{
    int i;
    COMMAND *command;
    char *word;

    np_set_counter(0);

    i = 0;
    while (line[i] && whitespace(line[i]))
        i++;
    word = line + i;

    while (line[i] && !whitespace(line[i]))
        i++;

    if (line[i])
        line[i++] = '\0';

    command = find_command(word);
    if (!command)
    {
        fprintf(stderr, "%s: No such command\n", word);
        return(-1);
    }

    while (whitespace(line[i]))
        i++;

    word = line + i;
    return ((*(command->func))(word));
}

COMMAND *
find_command(name)
    char *name;
{
    int i = 0;
    for (i = 0; commands[i].name; i++)
        if (strcmp(name, commands[i].name) == 0)
            return(&commands[i]);

    return ((COMMAND *)NULL);
}

int
cli_make(arg)
    char *arg;
{
    if (!arg)
        return 0;
    sprintf(clicom, "make %s", arg);
    return (system(clicom));
}


int
cli_list(arg)
    char *arg;
{
    if (!arg)
        arg = "";

    sprintf(clicom, "ls -l %s", arg);
    return (system(clicom));
}

int
cli_cd(arg)
    char *arg;
{
    if (chdir(arg) == -1)
    {
        perror(arg);
        return 1;
    }

    cli_pwd("");
    return (0);
}

int
cli_rename(arg)
    char *arg;
{
    return 0;
}

int
cli_help(arg)
    char *arg;
{
    int i = 0;
    int printed = 0;
    for(i = 0; commands[i].name; i++)
    {
        if (!*arg || (strcmp(arg, commands[i].name) == 0))
        {
            printf("%s\t\t%s.\n", commands[i].name, commands[i].doc);
            printed++;
        }
    }
    if (!printed)
    {
        printf(" No commands match '%s'. Available commands are:\n", arg);
        for (i = 0; commands[i].name; i++)
        {
            if(printed == 6)
            {
                printed = 0;
                printf("\n");
            }

            printf(" %s\t", commands[i].name);
            printed++;
        }

        if (printed)
            printf("\n");
    }
    return 0;
}

int
cli_delete(arg)
    char *arg;
{
    return 0;
}

int
cli_pwd(arg)
    char *arg;
{
    char dir[1024], *s;

    s = getwd(dir);
    if (s == 0)
    {
        fprintf(stdout, "Error getting pwd: %s\n", dir);
        return 1;
    }
    fprintf(stdout, "%s\n", dir);
    return 0;
}

int
cli_execeiop(arg)
    char *arg;
{
    int argvlen;
    fix_cmd_arg(arg, cmd, &argvlen);
    set_command(PACKET_EXECIOP);
    set_cmdbuf(cmd);
    return 0;
}

int
cli_execee(arg)
    char *arg;
{
    int argvlen;
    fix_cmd_arg(arg, cmd, &argvlen);
    set_command(PACKET_EXECPS2);
    set_cmdbuf(cmd);
    return 0;
}

int
cli_reset(arg)
    char *arg;
{
    set_command(PACKET_RESET);
    return 0;
}

int
cli_benchmark() {
    set_command(PACKET_BENCHMARK);
    return 0;
}

int
cli_quit(arg)
    char *arg;
{
    set_command(PACKET_QUIT);
    doloop = 0;
    return 0;
}

int
cli_status(arg)
    char *arg;
{
    if ( log_to_file )
        printf(" logging to file\n");
    else
        printf(" logging to stdout\n");
    if ( VERBOSE )
        printf(" verbose mode is on\n");
    else
        printf(" verbose mode is off\n");
    return 0;
}


int
cli_reconnect(arg)
    char *arg;
{
    return 0;
}

int
cli_log(arg)
    char *arg;
{
    trim(arg);
    if ( (strcmp(arg, "stdout"))==0 || !*arg)
    {
        if ( log_to_file )
        {
            if (VERBOSE)
                printf(" closing log file fd\n");
            close(log_f_fd);
        }
        if (VERBOSE)
            printf(" loggint to stdout\n");
        log_to_file = 0;
    } else {
        if (VERBOSE)
            printf(" open file %s for logging\n", arg);
        log_to_file = 1;
        log_f_fd = open(arg, LOG_F_CREAT, LOG_F_MODE);
    }
    return 0;
}

int
cli_verbose()
{
    if (VERBOSE) {
        printf(" Verbose off\n");
        VERBOSE = 0;
    } else
    {
        VERBOSE = 1;
        printf(" Verbose on\n");
    }
    return 0;
}

int
cli_debug()
{
    if (DEBUG) {
        DEBUG = 0;
        naplink_debug(0);
        printf(" debug off\n");
    } else
    {
        DEBUG = 1;
        naplink_debug(1);
        printf(" debug on\n");
    }
    return 0;
}

int
cli_dump(arg)
    char *arg;
{
    trim(arg);
    if (strcmp(arg, "screen")){
        printf(" exception dump to screen\n");

    } else if (strcmp(arg, "console")){
        printf(" exception dump to console\n");

    }
}

/*
 * Readline init.
 */
int
initialize_readline(void)
{
    rl_bind_key_in_map(META ('p'), cli_pwd, emacs_standard_keymap);
    rl_bind_key_in_map(META ('q'), cli_quit, emacs_standard_keymap);
    rl_bind_key_in_map(META ('r'), cli_reset, emacs_standard_keymap);
    rl_bind_key_in_map(META ('s'), cli_status, emacs_standard_keymap);
    rl_bind_key_in_map(META ('v'), cli_verbose, emacs_standard_keymap);
    rl_readline_name = "napc";
    rl_attempted_completion_function = command_completion;
    rl_ignore_some_completions_function = filename_completion_ignore;
    if (strcmp(pksh_history, "") != 0){
        if(read_history(pksh_history) != 0){
            perror(pksh_history);
        }
    }
    rl_callback_handler_install(prompt, cli_handler);
    return 0;
}

char *
command_generator(text, state)
    char *text;
    int state;
{
    static int list_index, len;
    char *name;

    if (!state)
    {
        list_index = 0;
        len = strlen(text);
    }

    while (name = commands[list_index].name )
    {
        list_index++;
        if (strncmp(name, text, len) == 0)
            return(dupstr(name));
    }
    return((char *)NULL); 
}

void
read_config(void)
{
    static char key[MAXPATHLEN];
    static char value[MAXPATHLEN];
    static char line[MAXPATHLEN];
    char *ptr = value;
    FILE *fd;
    if(get_home(line)) {
        read_pair(line, key, value);
    } else {
        strcpy(value, "./");
    }

    strcat(value, "/.npshrc");
    if ( (fd = fopen(value, "rb")) == NULL ) {
        perror(value);
        return;
    }

    while((read_line(fd, line)) != -1) {
        if (line[0] == '#') {
            continue;
        }
        read_pair(line, key, value);
        trim(key);
        ptr = stripwhite(value);
        if (strcmp(key, "log") == 0) {
            if (strcmp(ptr, "") == 0) {
                cli_log(stdout);
            } else {
                cli_log(ptr);
            }
        } else if (strcmp(key, "verbose") == 0) {
            if (strcmp(ptr, "yes") == 0) {
                printf(" Verbose mode on\n");
                VERBOSE = 1;
            }
        } else if (strcmp(key, "debug") == 0) {
            if (strcmp(ptr, "yes") == 0) {
                DEBUG = 1;
            }
        } else if (strcmp(key, "histfile") == 0) {
            if (strcmp(ptr, "") != 0) {
                strcpy(pksh_history, ptr);
            }
        } else if (strcmp(key, "path") == 0) {
            if (strcmp(ptr, "") != 0) {
                np_set_path(path_split(ptr));
            }
        } else if (strcmp(key, "suffix") == 0) {
            if (strcmp(ptr, "") != 0) {
                common_set_suffix(path_split(ptr));
            }
        } else if (strcmp(key, "home") == 0) {
            if (strcmp(ptr, "") != 0) {
                if (chdir(ptr) == -1) {
                    perror(ptr);
                }
            }
        } else if (strcmp(key, "timeout") == 0) {
            if (strcmp(ptr, "") != 0) {
                timeout = atoi(ptr);
            }
        }
    }
    return;
}
