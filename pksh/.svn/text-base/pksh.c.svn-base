/*
 * pukklink/ps2link Shell v2.1
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
#include "pksh.h"
#include "netfsproto_core.h"
#ifdef FTP
#include "ftplib.h"
#endif

static char dst_ip[16] = "192.168.0.10";
static char src_ip[16] = "0.0.0.0";
static char pksh_history[MAXPATHLEN];
static int time1, time2, time_base = 0;
static struct timeval benchtime;
static char device[MAXPATHLEN];
static char dir[MAXPATHLEN];
static char filename[MAXPATHLEN];
static char device2[MAX_PATH];
static char dir2[MAX_PATH];
static char filename2[MAX_PATH];
#ifdef FTP
static netbuf *ftpConn = NULL;
#endif

int
main(int argc, char* argv[])
{
    int ret;
    // poll client stuff
    int i, j, maxi, connfd, sockfd;
    int maxfd;
    struct sockaddr_in cliaddr;
    struct timeval clienttimeout;
    int nready;
    socklen_t clilen;
    //
    int client[MAX_CLIENTS];
    fd_set master_set, readset, clientset;
    VERBOSE = 0;
    printf("pksh version 2.1\n");
    read_config();
    if (argc == 2) {
        strcpy(dst_ip, argv[1]);
        dst_ip[strlen(argv[1])] = '\0';
    }
    clienttimeout.tv_sec = 0;
    clienttimeout.tv_usec = USEC;

    printf(" Connecting to %s", dst_ip);
    fflush(stdout);
    pko_srv_fd = cmd_listener(dst_ip, SRV_PORT, 60);
    if (pko_srv_fd < 0) {
        printf(", failed\n");
        return 1;
    }

    // fast check if ps2_netfs is here
    ps2_netfs_fd = ps2netfs_open(dst_ip, PS2NETFS_LISTEN_PORT, 2);
    if (ps2_netfs_fd > 0) {
        printf("\n PS2NetFS service found!\n");
    }

    // client stuff
    for(i = 0; i < MAX_CLIENTS; i++) {
        client[i] = -1;
    }
    FD_ZERO(&clientset);
    // end client stuff

    printf("\n");

    if (DUMPSCREEN) {
        pko_cmd_con(dst_ip, CMD_PORT);
        pko_dump2screen_req(cmd_fd);
    } else {
        pko_cmd_con(dst_ip, CMD_PORT);
        pko_dump2pc_req(cmd_fd);
    }

    log_fd = log_listener(src_ip, LOG_PORT);

    if ( log_fd < 0 ) {
        perror("");
        printf("Unable to start log service!\n");
        return 1;
    }
    pksh_srv_fd = pko_srv_setup(src_ip, SRV_PORT);
    if ( pksh_srv_fd < 0 ) {
        perror("");
        printf("Unable to start command server!\n");
        return 1;
    }
    FD_ZERO(&master_set);
    FD_SET(0, &master_set);
    FD_SET(pko_srv_fd, &master_set);
    FD_SET(log_fd, &master_set);
    FD_SET(pksh_srv_fd, &master_set);
    client[0] = 0;
    client[1] = pko_srv_fd;
    client[2] = log_fd;
    client[3] = pksh_srv_fd;
    maxfd = pksh_srv_fd;
    maxi = 3;

    initialize_readline();

    while(doloop) {
        readset = master_set;
        ret = select(maxfd+1, &readset, NULL, NULL, NULL);
        if ( ret < 0 )
        {
            if ( FD_ISSET(0, &readset) ) {
                continue;
            }
            perror("select");
            break;
        } else if (ret == 0) {
            /* no file desc are ready, lets move on in life */
        } else {
            for(i = 0; i <= maxi; i++) {
                if ( (sockfd = client[i]) < 0) {
                    continue;
                }
                if ( !FD_ISSET(sockfd, &readset) ) {
                    continue;
                }
                if ( sockfd == log_fd ) {
                    pko_log_read(log_fd);
                } else if (sockfd == 0) {
                    rl_callback_read_char();
                } else if(sockfd == pko_srv_fd) {
                    pko_srv_read(pko_srv_fd);
                } else if (sockfd == pksh_srv_fd) {
                    clilen = sizeof(cliaddr);
                    connfd = accept(pksh_srv_fd, (struct sockaddr *)&cliaddr, &clilen);
                    for(j = 0; i<FD_SETSIZE; j++) {
                        if(client[j] < 0) {
                            client[j] = connfd;
                            break;
                        }
                    }
                    FD_SET(connfd, &master_set);
                    if(connfd > maxfd) {
                        maxfd = connfd;
                    }
                    if ( j > maxi ) {
                        maxi = j;
                    }
                    if (--nready <= 0) {
                        continue;
                    }
                } else {
                    if ( pko_srv_read(sockfd) < 0 ) {
                        close(sockfd);
                        FD_CLR(sockfd, &master_set);
                        client[i] = -1;
                        maxi--;
                    }
                }
            }
        }
    }

    rl_callback_handler_remove();
    if ( (ret = close(pko_srv_fd)) == -1 ) {
        perror("pko_srv_fd");
    }
    if ( (ret = close(log_fd)) == -1 ) {
        perror("log_fd");
    }
    if ( log_to_file ) {
        if ((ret = close(log_f_fd)) == -1)
            perror("log_f_fd");
    }
    if ( ps2_netfs_fd > 0 ) {
        if ((ret = close(ps2_netfs_fd)) == -1) {
            perror("");
        }
    }

#ifdef FTP
	if (ftpConn) {
		FtpQuit(ftpConn);
	}
#endif

    if (strcmp(pksh_history, "") != 0 ) {
        if ( (ret = write_history(pksh_history)) != 0) 
            perror("write_history");
    }
    printf("\n");
    return(0);
}

int
pko_log_read(int fd)
{
    int ret;
    int size = sizeof(struct sockaddr_in);
    static char buf[1024];
    static int loc = 0;
    struct sockaddr_in dest_addr;

    if(loc == 0)
       memset(buf, 0x0, sizeof(buf));

    memset(&(dest_addr), '\0', size);

    /* Receive from, size must be at least 1 smaller 
       than buffer for a NULL */
    ret = recvfrom(
        fd, buf + loc, sizeof(buf) - loc - 1, 0,
        (struct sockaddr *)&dest_addr, &size
        );

    if (ret == -1) {
        perror("read");
    }

    loc += ret;

    if (log_to_file) {
        write(log_f_fd, buf, strlen(buf));
	loc = 0;
	/* log to file isn't line orientated */
    } else {
       if(((strchr(buf, '\n')) || (sizeof(buf) - loc - 1) <= 0))
       {
	  /* Ideally we want entire lines but just in case check 
	     for NL at end of string */
	  int str_size = strlen(buf);
	  if(buf[str_size - 1] != '\n')
	     buf[str_size - 1] = '\n';

	  write_log_line(buf);
	  fflush(stdout);
	  loc = 0;
       }
    }
    return ret;
}

int
pko_srv_read(int fd) {
    int length = 0;
    int ret = 0;
    unsigned int cmd;
    unsigned short hlen;
    pkt_hdr *header;
    
    length = pko_recv_bytes(fd, &recv_packet[0], sizeof(pkt_hdr));
    if ( length < 0 ) {
        return length;
    } else if ( length == 0 ) {
        if ( fd == pko_srv_fd ) {
            close(fd);
            change_prompt();
            while(1) {
                sleep(1);
                pko_srv_fd = cmd_listener(dst_ip, SRV_PORT, 1);
                if (pko_srv_fd > 0) {
                    close(ps2_netfs_fd);
                    ps2_netfs_fd = -1;
                    break;
                }
            }
            change_prompt();
        } else {
            return -1;
        }
    } else {
        header = (pkt_hdr *)&recv_packet[0];
        cmd = ntohl(header->cmd);
        hlen = ntohs(header->len);
        ret = pko_recv_bytes(fd, &recv_packet[length],
            hlen - sizeof(pkt_hdr));

        switch (cmd) {
            case OPEN_CMD:
                if (VERBOSE) {
                    gettimeofday(&benchtime, NULL);
                    time1=(benchtime.tv_sec - time_base)*USEC+benchtime.tv_usec;
                }
                pko_open_file(&recv_packet[0]);
                break;
            case CLOSE_CMD:
                if (VERBOSE)
                {
                    gettimeofday(&benchtime, NULL);
                    time2=(benchtime.tv_sec - time_base)*USEC+benchtime.tv_usec;
                    printf(" took %2.3fs\n", ((float)(time2-time1)/(float)USEC));
                }
                pko_close_file(&recv_packet[0]);
                break;
            case READ_CMD:
                pko_read_file(&recv_packet[0]);
                break;
            case WRITE_CMD:
                pko_write_file(&recv_packet[0]);
                break;
            case LSEEK_CMD:
                pko_lseek_file(&recv_packet[0]);
                break;
            case DOPEN_CMD:
                pko_open_dir(&recv_packet[0]);
                break;
            case DREAD_CMD:
                pko_read_dir(&recv_packet[0]);
                break;
            case DCLOSE_CMD:
                pko_close_dir(&recv_packet[0]);
                break;
            case DUMPMEM_CMD:
                dumpmem( 
                    ((pkt_memio_req*)
                        &recv_packet[0])->argv,
                    (unsigned int)ntohl(((pkt_memio_req*)
                            &recv_packet[0])->offset),
                    (unsigned int)ntohl(((pkt_memio_req*)
                            &recv_packet[0])->size)
                    );
                break;
            case DUMPREGS_CMD:
                dumpregs(
                    ((pkt_dumpregs_req*)&recv_packet[0])->argv,
                    ntohl(((pkt_dumpregs_req*)&recv_packet[0])->regs)
                    );
                break;
            case GSEXEC_CMD:
                cli_gsexec(
                    ((pkt_gsexec_req*)&recv_packet[0])->file
                    );
            case STOPVU_CMD:
                pko_cmd_con(dst_ip, CMD_PORT);
                pko_stop_vu(cmd_fd, 
                    ntohs(((pkt_stopvu_req *)
                            &recv_packet[0])->vpu)
                    );
                break;
            case STARTVU_CMD:
                pko_cmd_con(dst_ip, CMD_PORT);
                pko_start_vu(cmd_fd, 
                    ntohs(((pkt_startvu_req *)
                            &recv_packet[0])->vpu)
                    );
                break;
            case EXECEE_CMD:
                cli_execee(
                    ((pkt_execee_req *)&recv_packet[0])->argv
                    );
                break;
            case EXECIOP_CMD:
                cli_execeiop(
                    ((pkt_execiop_req*)&recv_packet[0])->argv
                    );
                break;
            case RESET_CMD:
                cli_reset();
                break;
            default:
                printf(" unknown fileio command received (%x)\n", header->cmd);
                break;
        }
    }
    return 0;
}

/* 
 *      CLI commands
 */
static char clicom[1024];

int
execute_line(line)
    char *line;
{
    int i, ret;
    COMMAND *command;
    char *word;
    char *arg = strdup(line);

    i = 0;
    while (line[i] && whitespace(line[i]))
        i++;
    word = line + i;

    while (line[i] && !whitespace(line[i]))
        i++;

    if (line[i])
        line[i++] = '\0';

    command = find_command(word);

    if (!command) {
        ret = system(arg);
        free(arg);
        return ret;
    }

    while (whitespace(line[i]))
        i++;

    word = line + i;
    free(arg);
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
    int ret, fd;
    if (!arg)
        arg = "";

    split_filename(device, dir, filename, arg);
    if(device[0] != NULL) {
        ps2netfs_dirent_t dirent;
        if ( ps2_netfs_fd < 0 ) {
            ps2_netfs_fd = ps2netfs_open(dst_ip, PS2NETFS_LISTEN_PORT, 2);
            if ( ps2_netfs_fd < 0 ) {
                printf("PS2Net FS connection failed, make sure ps2netfs.irx is loaded\n");
                return 0;
            }
        }
        ret = fd = ps2netfs_req_dopen(arg);
        if (ret < 0) {
            close(ps2_netfs_fd);
            ps2_netfs_fd = -1;
            printf("failed to open %s\n", arg);
            return -1;
        }
        if ( ret > 0 ) {
            printf("[%s]\n", arg);
        }
        while((ps2netfs_req_dread(fd, &dirent)) > 0) {
            ps2netfs_showStat(&dirent);
            printf(" %-20s \n", dirent.name);
        }
        if ( fd > 0 ) {
            ps2netfs_req_dclose(fd);
        }
        return 0;
    } else {
        sprintf(clicom, "ls -l %s", arg);
        return (system(clicom));
    }
}

int
cli_cd(arg)
    char *arg;
{
    char line[MAXPATHLEN];
    char key[MAXPATHLEN];
    char value[MAXPATHLEN];
    if (strlen(arg) == 0) {
        get_home(line);
        read_pair(line, key, value);
        chdir(value);
        cli_pwd();
        return 0;
    }
    if (chdir(arg) == -1) {
        perror(arg);
        return 1;
    }
    cli_pwd();
    return (0);
}

int
cli_gsexec(arg)
    char *arg;
{
    int size, ret, argvlen;
    unsigned char argv[MAX_PATH];
    trim(arg);
    size = size_file(arg);
    if ( size > 16384 ) {
        printf("Data file too big\n");
        return -1;
    } else if ( size == 0 ) {
        printf("Data file is zero byte large\n");
        return -1;
    }

    fix_cmd_arg(argv, arg, &argvlen);

    pko_cmd_con(dst_ip, CMD_PORT);
    ret = pko_gsexec_req(cmd_fd, argv, size);
    if ( ret < 0) {
        perror(" gsexec failed");
    }
    return 0;
}

int
cli_help(arg)
    char *arg;
{
    int i = 0;
    int printed = 0;
    for(i = 0; commands[i].name; i++) {
        if (!*arg || (strcmp(arg, commands[i].name) == 0)) {
            printf("%-10s  %s.\n", commands[i].name, commands[i].doc);
            printed++;
        }
    }
    if (!printed) {
        printf(" No commands match '%s'. Available commands are:\n", arg);
        for (i = 0; commands[i].name; i++) {
            if(printed == 6) {
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
cli_pwd()
{
    char dir[1024], *s;
    s = getwd(dir);
    if (s == 0) {
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
    int ret;
    unsigned int argc, argvlen;
    unsigned char argv[MAX_PATH];
    state = 1;
    argc = fix_cmd_arg(argv, arg, &argvlen);
    pko_cmd_con(dst_ip, CMD_PORT);
    ret = pko_execiop_req(cmd_fd, argv, argvlen, argc);
    if ( ret < 0) {
        perror(" execiop failed");
    }
    return 0;
}

int
cli_execee(arg)
    char *arg;
{
    int ret;
    unsigned int argc, argvlen;
    unsigned char argv[MAX_PATH];
    if ( state == 1 ) {
        cli_reset("");
        close(pko_srv_fd);
        change_prompt();
        while(1) {
            sleep(1);
            pko_srv_fd = cmd_listener(dst_ip, SRV_PORT, 1);
            if (pko_srv_fd > 0) {
                break;
            }
        }
        change_prompt();
    }
    state = 1;
    argc = fix_cmd_arg(argv, arg, &argvlen);
    pko_cmd_con(dst_ip, CMD_PORT);
    ret = pko_execee_req(cmd_fd, argv, argvlen, argc);
    if ( ret < 0) {
        perror(" execee failed");
    }
    return 0;
}

int
cli_dumpregs(arg)
    char *arg;
{
    int ret;
    unsigned int regs = DUMP_REG_MAX;
    char file[MAX_PATH];
    char *ptr;
    trim(arg);
    if ((ptr = strchr(arg, ' ')) == NULL) {
        return -1;
    }
    strncpy(file, arg, strlen(arg)-strlen(ptr));
    file[strlen(arg)-strlen(ptr)] = '\0';
    if ((ptr = strchr(arg, ' ')) == NULL) {
        return -1;
    }

    regs = get_register_index(ptr+1, ptr-arg);
    ret = dumpregs(file, regs);
    if ( ret < 0) {
        perror(" dumpregs failed");
    }
    return 0;
}

int
cli_debug() {
    if (pko_debug()) {
        pko_set_debug(0);
        printf(" Debug off\n");
    } else {
        pko_set_debug(1);
        printf(" Debug on\n");
    }
    return 0;
}

int
dumpregs(char *file, unsigned int regs) {
    char argv[MAX_PATH];
    arg_prepend_host(argv, file);
    pko_cmd_con(dst_ip, CMD_PORT);
    return pko_dumpregs_req(cmd_fd, argv, regs);
}

int
cli_dumpmem(arg)
    char *arg;
{
    unsigned int size = 0;
    unsigned int offset = 0;
    int ret;
    char file[MAX_PATH];
    char *ptr;
    trim(arg);
    if ((ptr = strchr(arg, ' ')) == NULL) {
        return -1;
    }
    strncpy(file, arg, strlen(arg)-strlen(ptr));
    file[strlen(arg)-strlen(ptr)] = '\0';
    offset = (unsigned int)strtol(ptr, (char **)NULL, 0);
    ptr = trim(ptr);
    if ((ptr = strchr(ptr, ' ')) == NULL) {
        return -1;
    }
    size = (unsigned int)strtol(ptr, (char **)NULL, 0);
    ret = dumpmem(file, offset, size);
    if ( ret < 0) {
        perror(" dumpmem failed");
    }
    return 0;
}

int
dumpmem(char *file, unsigned int offset, unsigned int size) {
    char argv[MAX_PATH];
    pko_cmd_con(dst_ip, CMD_PORT);
    arg_prepend_host(argv, file);
    return pko_dumpmemory_req(cmd_fd, argv, offset, size);
}

int
cli_exception(arg)
    char *arg;
{
    trim(arg);
    pko_cmd_con(dst_ip, CMD_PORT);
    if (strcmp(arg, "console") == 0) {
        DUMPSCREEN = 0;
        pko_dump2pc_req(cmd_fd);
    } else if (strcmp(arg, "screen") == 0) {
        DUMPSCREEN = 1;
        pko_dump2screen_req(cmd_fd);
    } else {
        printf("Unknown arg to dump: \"%s\"\n", arg);
    }
    return 0;
}

int
cli_vumem(void) {
    // if no tmpfile echo "sorry set tempfile in .pkshrc"
    // dump vu mem to tmpfile
    // load tmpfile and display, we need to figure out the window height.
    return 0;
}

int
cli_vustart(char *arg) {
    int vpu = strtol(arg, (char **)NULL, 10);
    pko_cmd_con(dst_ip, CMD_PORT);
    if ( vpu != 0 && vpu != 1 ) {
        return -1;
    }
    pko_start_vu(cmd_fd, vpu);
    return 0;
}

int
cli_vustop(char *arg) {
    int vpu = strtol(arg, (char **)NULL, 10);
    pko_cmd_con(dst_ip, CMD_PORT);
    if ( vpu != 0 && vpu != 1 ) {
        return -1;
    }
    pko_stop_vu(cmd_fd, vpu);
    return 0;
}

/* int */
/* check_integer(char *arg) { */
/*     long long_var; */
/*     char *end_ptr; */
/*     long_var = strtol(arg, &end_ptr, 0); */

/*     if (ERANGE == errno) { */
/*     } else if (long_var > INT_MAX) { */
/*     } else if (long_var < INT_MIN) { */
/*     } else if (end_ptr == arg) { */
/*     } else { */
/*         return 1; */
/*     } */
/*     return 0; */
/* } */

int
cli_poweroff() {
    pko_cmd_con(dst_ip, CMD_PORT);
    pko_poweroff_req(cmd_fd);
    return 0;
}

int
cli_status() {
    printf(" TCP srv fd = %d\n", pko_srv_fd);
    printf(" UDP log fd = %d\n", log_fd);
    printf(" PKSH cmd fd = %d\n", pksh_srv_fd);
    printf(" PS2NetFS fd = %d\n", ps2_netfs_fd);
    if ( log_to_file )
        printf(" Logging to file\n");
    else
        printf(" Logging to stdout\n");
    if ( VERBOSE )
        printf(" Verbose mode is on\n");
    else
        printf(" Verbose mode is off\n");

    if(pko_debug()) {
        printf(" Debug is on\n");
    } else {
        printf(" Debug is off\n");
    }

    if ( DUMPSCREEN ) {
        printf(" Exception dumps to screen\n");
    } else {
        printf(" Exception dumps to console\n");
    }
    return 0;
}

int
cli_quit() {
    doloop = 0;
    return 0;
}

int
cli_reconnect() {
    close(pko_srv_fd);
    pko_srv_fd = cmd_listener(dst_ip, SRV_PORT, 60);
    close(ps2_netfs_fd);
    ps2_netfs_fd = -1;
    return 0;
}

int
cli_reset() {
    state = 0;
    pko_cmd_con(dst_ip, CMD_PORT);
    pko_reset_req(cmd_fd);
    close(ps2_netfs_fd);
    ps2_netfs_fd = -1;
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
                printf(" Closing log file fd\n");
            close(log_f_fd);
        }
        if (VERBOSE)
            printf(" Logging to stdout\n");
        log_to_file = 0;
    } else {
        if (VERBOSE)
            printf(" Open file %s for logging\n", arg);
        log_to_file = 1;
        log_f_fd = open(arg, LOG_F_CREAT, LOG_F_MODE);
    }
    return 0;
}

int
cli_verbose() {
    if (VERBOSE) {
        printf(" Verbose off\n");
        VERBOSE = 0;
    } else {
        VERBOSE = 1;
        printf(" Verbose on\n");
    }
    return 0;
}

int
cli_writemem(char *arg) {
    int ret;
    char file[MAX_PATH];
    char argv[MAX_PATH];
    int size, fd, offset;
    char *ptr;
    trim(arg);

    if ((ptr = strchr(arg, ' ')) == NULL) {
        return -1;
    }
    strncpy(file, arg, strlen(arg)-strlen(ptr));
    file[strlen(arg)-strlen(ptr)] = '\0';
    offset = (unsigned int)strtol(ptr, (char **)NULL, 0);

    fd = open(file, O_RDONLY);
    size = lseek(fd, 0, SEEK_END);
    close(fd);

    pko_cmd_con(dst_ip, CMD_PORT);
    arg_prepend_host(argv, file);
    ret = pko_writemem_req(cmd_fd, argv, offset, size);
    if ( ret < 0 ) {
        perror(" writemem failed");
    }
    return 0;
}

int
cli_setroot(arg)
    char *arg;
{
    pko_set_root(arg);
    return(0);
}

/*
 * PS2 NetFS commands
 */
int
cli_copy(char *arg) {
    int fd0, fd1, total, result;
    int argc;
    char *argv[MAX_ARGV];
    int size;
    char buffer[65536];

    argc = build_argv(argv, arg);
    if ( argc < 2 ) {
        printf("Not enough arguments\n");
        return 0;
    }
    split_filename(device, dir, filename, argv[0]);
    split_filename(device2, dir2, filename2, argv[1]);

    free_argv(argv, argc);

    if ( device[0] != NULL ) {
        if ( ps2_netfs_fd < 0 ) {
            ps2_netfs_fd = ps2netfs_open(dst_ip, PS2NETFS_LISTEN_PORT, 2);
            if ( ps2_netfs_fd < 0 ) {
                printf("PS2Net FS connection failed, make sure ps2netfs.irx is loaded\n");
                return 0;
            }
        }
        fd0 = open(argv[1], O_RDWR | O_CREAT, 0644);
        if(fd0 < 0) {
            printf("failed to open %s\n", argv[1]);
            return -1;
        }
        fd1 = ps2netfs_req_open(argv[0], PS2_O_RDONLY);
        if(fd1 < 0) {
            printf("failed to open :%s:\n", argv[0]);
            return -1;
        }
        size = ps2netfs_req_lseek(fd1, 0, FIO_LSEEK_END);
        ps2netfs_req_lseek(fd1, 0, FIO_LSEEK_SET);
        printf("\n [%s --> %s]\n\n  Progress: ", argv[0], argv[1]);
        while(size > 0) {
            printf("#");
            ps2netfs_req_read(fd1, buffer, sizeof(buffer));
            if(size < sizeof(buffer)) {
                write(fd0, buffer, size);
            } else {
                write(fd0, buffer, sizeof(buffer));
            }
            size -= sizeof(buffer);
        }
        printf("\n");
        ps2netfs_req_close(fd1);
        close(fd0);
        return 0;
    } else if (device2[0] != NULL ) {
        if ( ps2_netfs_fd < 0 ) {
            ps2_netfs_fd = ps2netfs_open(dst_ip, PS2NETFS_LISTEN_PORT, 2);
            if ( ps2_netfs_fd < 0 ) {
                printf("PS2Net FS connection failed, make sure ps2netfs.irx is loaded\n");
                return 0;
            }
        }
        fd0 = open(argv[0], O_RDONLY);
        if(fd0 < 0) {
            printf("failed to open %s\n", argv[0]);
            return -1;
        }
        fd1 = ps2netfs_req_open(argv[1], PS2_O_WRONLY | PS2_O_CREAT);
        if(fd1 < 0) {
            printf("failed to open :%s:\n", argv[1]);
            return -1;
        }
        size = lseek(fd0, 0, SEEK_END);
        lseek(fd0, 0, SEEK_SET);
        printf("\n [%s --> %s]\n\n  Progress: ", argv[0], argv[1]);
        total = 0;
        while(total < size) {
            printf("#\n");
            result = read(fd0, buffer, sizeof(buffer));
            if (result < 0) {
            }
            result = ps2netfs_req_write(fd1, buffer, result);
            if (result < 0) {
            }
            total += result;
        }
        printf("\n");
        ps2netfs_req_close(fd1);
        close(fd0);       
        return 0;
    } else {
        sprintf(clicom, "cp %s", arg);
        return (system(clicom));
    }
}

int
cli_devlist(void) {
    int numdevs, i;
    char devlist[256];
    char *ptr = devlist;

    if ( ps2_netfs_fd < 0 ) {
        ps2_netfs_fd = ps2netfs_open(dst_ip, PS2NETFS_LISTEN_PORT, 2);
        if ( ps2_netfs_fd < 0 ) {
            printf("PS2Net FS connection failed, make sure ps2netfs.irx is loaded\n");
            return 0;
        }
    }

    numdevs = ps2netfs_req_devlist(devlist);
    printf("Device list.\n");
    for(i = 0; i < numdevs; i++) {
        printf(" dev %02d : '%s'\n", i, ptr);
        ptr += strlen(ptr)+1;
    }
    return 0;
}

int
cli_ps2umount(char *arg) {
    int ret;
    if ( ps2_netfs_fd < 0 ) {
        ps2_netfs_fd = ps2netfs_open(dst_ip, PS2NETFS_LISTEN_PORT, 2);
        if ( ps2_netfs_fd < 0 ) {
            printf("PS2Net FS connection failed, make sure ps2netfs.irx is loaded\n");
            return 0;
        }
    }
    ret = ps2netfs_req_umount(arg, 0);
    if ( ret < 0 ) {
        printf("unmount failed, ret = %d\n", ret);
    }
    return ret;
}

int
cli_ps2sync(char *arg) {
    int ret;
    if ( ps2_netfs_fd < 0 ) {
        ps2_netfs_fd = ps2netfs_open(dst_ip, PS2NETFS_LISTEN_PORT, 2);
        if ( ps2_netfs_fd < 0 ) {
            printf("PS2Net FS connection failed, make sure ps2netfs.irx is loaded\n");
            return 0;
        }
    }
    ret = ps2netfs_req_sync(arg, 0);
    return 0;
}

int
cli_rmdir(char *arg) {
    int ret;
    if (!arg)
        arg = "";
    split_filename(device, dir, filename, arg);
    if(device[0] != NULL) {
        if ( ps2_netfs_fd < 0 ) {
            ps2_netfs_fd = ps2netfs_open(dst_ip, PS2NETFS_LISTEN_PORT, 2);
            if ( ps2_netfs_fd < 0 ) {
                printf("PS2Net FS connection failed, make sure ps2netfs.irx is loaded\n");
                return 0;
            }
        }
        ret = ps2netfs_req_rmdir(arg);
        if ( ret < 0 ) {
            printf("rmdir failed, ret = %d\n", ret);
        }
        return 0;
    } else {
        sprintf(clicom, "rm -r %s", arg);
        return (system(clicom));
    }
}

int
cli_ps2format(char *arg) {
    int ret;
    if ( ps2_netfs_fd < 0 ) {
        ps2_netfs_fd = ps2netfs_open(dst_ip, PS2NETFS_LISTEN_PORT, 2);
        if ( ps2_netfs_fd < 0 ) {
            printf("PS2Net FS connection failed, make sure ps2netfs.irx is loaded\n");
            return 0;
        }
    }
    ret = ps2netfs_req_format(arg, 0);
    if ( ret < 0 ) {
        printf("format failed, ret = %d\n", ret);
    }
    return 0;
}

int
cli_mkdir(char *arg) {
    int ret;
    if (!arg)
        arg = "";
    split_filename(device, dir, filename, arg);
    if(device[0] != NULL) {
        if ( ps2_netfs_fd < 0 ) {
            ps2_netfs_fd = ps2netfs_open(dst_ip, PS2NETFS_LISTEN_PORT, 2);
            if ( ps2_netfs_fd < 0 ) {
                printf("PS2Net FS connection failed, make sure ps2netfs.irx is loaded\n");
                return 0;
            }
        }
        ret = ps2netfs_req_mkdir(arg);
        if ( ret < 0 ) {
            printf("mkdir failed, ret = %d\n", ret);
        }
        return 0;
    } else {
        sprintf(clicom, "mkdir %s", arg);
        return (system(clicom));
    }
}

int
cli_rename(char *arg) {
    int ret;
    if (!arg)
        arg = "";
    split_filename(device, dir, filename, arg);
    if(device[0] != NULL) {
        if ( ps2_netfs_fd < 0 ) {
            ps2_netfs_fd = ps2netfs_open(dst_ip, PS2NETFS_LISTEN_PORT, 2);
            if ( ps2_netfs_fd < 0 ) {
                printf("PS2Net FS connection failed, make sure ps2netfs.irx is loaded\n");
                return 0;
            }
        }
        ret = ps2netfs_req_delete(arg, 0);
        if ( ret < 0 ) {
            printf("rename failed, %d\n", ret);
        }
    } else {
        sprintf(clicom, "rm %s", arg);
        return (system(clicom));
    }
    return 0;
}

int
cli_ps2mount(char *arg) {
    int ret;
    if ( ps2_netfs_fd < 0 ) {
        ps2_netfs_fd = ps2netfs_open(dst_ip, PS2NETFS_LISTEN_PORT, 2);
        if ( ps2_netfs_fd < 0 ) {
            printf("PS2Net FS connection failed, make sure ps2netfs.irx is loaded\n");
            return 0;
        }
    }
    split_filename(device, dir, filename, arg);
    ret = ps2netfs_req_mount(device, filename, PS2NETFS_FIO_MT_RDWR, "", 0);
    if ( ret < 0 ) {
        printf("mount failed, ret = %d\n", ret);
    } else {
    }
    return 0;
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
    rl_readline_name = "pksh";
    rl_attempted_completion_function = command_completion;
    rl_ignore_some_completions_function = filename_completion_ignore;
    if (strcmp(pksh_history, "") != 0) {
        if (read_history(pksh_history) != 0) {
            perror(pksh_history);
        }
    }
    rl_callback_handler_install(get_prompt(), cli_handler);
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

    while ((name = commands[list_index].name) )
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

    strcat(value, "/.pkshrc");
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
        if (strcmp(key, "ip") == 0) {
            strcpy(dst_ip, ptr);
        } else if (strcmp(key, "log") == 0) {
            if (strcmp(ptr, "") == 0) {
                cli_log(stdout);
            } else {
                cli_log(ptr);
            }
        } else if (strcmp(key, "exception") == 0) {
            if (strcmp(ptr, "screen") == 0) {
                DUMPSCREEN = 1;
            } else {
                DUMPSCREEN = 0;
            }
        } else if (strcmp(key, "verbose") == 0) {
            if (strcmp(ptr, "yes") == 0) {
                printf(" Verbose mode on\n");
                VERBOSE = 1;
            }
        } else if (strcmp(key, "debug") == 0) {
            if (strcmp(ptr, "yes") == 0) {
                pko_set_debug(1);
            }
        } else if (strcmp(key, "histfile") == 0) {
            if (strcmp(ptr, "") != 0) {
                strcpy(pksh_history, ptr);
            }
        } else if (strcmp(key, "bind") == 0) {
            if (strcmp(ptr, "") != 0) {
                strcpy(src_ip, ptr);
            }
        } else if (strcmp(key, "path") == 0) {
            if (strcmp(ptr, "") != 0) {
                pko_set_path(path_split(ptr));
            }
        } else if (strcmp(key, "suffix") == 0) {
            if (strcmp(ptr, "") != 0) {
                common_set_suffix(path_split(ptr));
            }
        } else if (strcmp(key, "setroot") == 0) {
            if (strcmp(ptr, "") != 0) {
                pko_set_root(ptr);
            }
        } else if (strcmp(key, "logprompt") == 0) {
            if (strcmp(ptr, "") != 0) {
                /* pko_set_root(ptr); */
            }
        } else if (strcmp(key, "home") == 0) {
            if (strcmp(ptr, "") != 0) {
                if (chdir(ptr) == -1) {
                    perror(ptr);
                }
            }
        }
    }
    return;
}

#ifdef FTP
int
cli_ftp(char *arg) {
    int argc;
    char *argv[MAX_ARGV];
    FtpInit();

    argc = build_argv(argv, arg);
    if (argv[0] != NULL ) {
        if(!FtpConnect(argv[0], &ftpConn)) {
            free_argv(argv, argc);
            return -1;
        }
		printf("FTP connection to %s succesful\n", argv[0]);
    } else {
        // replace with ps2 ip from .pkshrc
        if(!FtpConnect("localhost", &ftpConn)) {
            return -1;
        }
		printf("FTP connection to %s succesful\n", "localhost");
    }
    free_argv(argv, argc);
    if(!FtpLogin("anonymous", "anonymous", ftpConn)) {
        printf("Login failed\n");
        return -1;
    }

	if(!FtpOptions(FTPLIB_CONNMODE, FTPLIB_PASSIVE, ftpConn)) {
		printf("Pasv failed\n");
	}
    /* prompt_ftp(); */
    return 0;
}

int
cli_ftpdir(char *arg) {
    int ret;
    if (!ftpConn) {
        if(cli_ftp(NULL)) {
        }
    }
    ret = FtpDir(NULL, arg, ftpConn);
    if (!ret) {
        printf("ftp error: %s\n", FtpLastResponse(ftpConn));
    }
    return 0;
}

int
cli_ftplist(char *arg) {
    int ret;
    if (!ftpConn) {
        if(cli_ftp(NULL)) {
        }
    }
    ret = FtpNlst(NULL, arg, ftpConn);
    if (!ret) {
        printf("ftp error: %s\n", FtpLastResponse(ftpConn));
    }
    return 0;
}

int
cli_ftpput(char *arg) {
    int ret;
    int argc;
    char *argv[MAX_ARGV];
    static char mode = 'I';
    argc = build_argv(argv, arg);
    if (!ftpConn) {
        if(cli_ftp(NULL)) {
        }
    }
    ret = FtpPut(argv[0], argv[1], mode, ftpConn);
    free_argv(argv, argc);
    if (!ret) {
        printf("ftp error: %s\n", FtpLastResponse(ftpConn));
        return -1;
    }
    return 0;
}

int
cli_ftpget(char *arg) {
    int ret;
    int argc;
    static char mode = 'I';
    char *argv[MAX_ARGV];
    argc = build_argv(argv, arg);
    if (!ftpConn) {
        if(cli_ftp(NULL)) {
        }
    }
    printf("argv[0] = %s\n", argv[0]);
    printf("argv[1] = %s\n", argv[1]);
    ret = FtpGet(argv[0], argv[1], mode, ftpConn);
    if (!ret) {
        printf("ftp error: %s\n", FtpLastResponse(ftpConn));
    }
    return 0;
}

int
cli_ftpdelete(char *arg) {
    int ret;
    if (!ftpConn) {
        if(cli_ftp(NULL)) {
        }
    }
    ret = FtpDelete(arg, ftpConn);
    if (!ret) {
        printf("ftp error: %s\n", FtpLastResponse(ftpConn));
    }
    return 0;
}

int
cli_ftpclose(void) {
    FtpQuit(ftpConn);
    printf("FTP connection closed\n");
    return 0;
}

int
cli_ftpsite(char *arg) {
    int ret;
    ret = FtpSite(arg, ftpConn);
    if (!ret) {
        printf("ftp error: %s\n", FtpLastResponse(ftpConn));
    }
    return 0;
}
#endif
