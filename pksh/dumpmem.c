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

#include "linkproto_core.h"
#include "linkproto_stub.h"
#include "common.h"
#include "batch.h"

char dst_ip[16];
char *local_ip = "127.0.0.1";

int
main(int argc, char **argv)
{
    fd_set master_set;
    int sock, ret, local;
    char ch;
    char *end_ptr;
    unsigned char file[MAX_PATH];
    unsigned int offset, size;
    memset(file, 0, MAX_PATH);

    if (argc < 2) {
        printf("Usage:\n"
               " %s <EE elf> arg1 .. argN\n\n", argv[0]);
        return 0;
    }

    while((ch = getopt(argc, argv, "h:")) != -1 ) {
        switch((char)ch) {
            case 'h':
                strncpy(dst_ip, optarg, 16);
                break;
            default:
                /* usage(); */
                break;
        }
    }

    // check if pksh is running, if so use it
    if ( (sock = ps2link_open(&sock, local_ip)) < 0 ) {
        // do we have an alternative ip ?
        if ( (sock = ps2link_open(&cmd_fd, dst_ip)) < 0 ) {
            printf("Unable to connect to ps2link server\n");
            return -1;
        }
        printf("Connected to %s ps2link server\n", dst_ip);
        local = 0;
    } else {
        local = 1;
        printf("Connected to local ps2link server ( pksh )\n");
    }

    // if we have device its most likely a full path
    if ( !arg_device_check(argv[optind]) ) {
        strcpy(file, getcwd(NULL, 0));
        strcat(file, "/");
        strcat(file, argv[optind]);
    } else {
        strcpy(file, argv[optind]);
    }
    optind++;

    offset = (unsigned int)strtol(argv[optind], &end_ptr, 0);
	if ( argv[optind] == end_ptr ) {
		printf("offset is not a valid number\n");
		/* usage(); */
		return 0;
	}
    optind++;
    size = (unsigned int)strtol(argv[optind], &end_ptr, 0);
	if ( argv[optind] == end_ptr ) {
		printf("offset is not a valid number\n");
		/* usage(); */
		return 0;
	}

    // if not local service file io
    if ( !local ) {
		close(sock);
		pko_cmd_con(dst_ip, CMD_PORT);
        ret = pko_dumpmemory_req(cmd_fd, file, offset, size);
		if (ret < 0) {
			printf("Sending execiop request failed\n");
		}
		pko_srv_fd = cmd_listener(dst_ip, SRV_PORT, 60);
        log_fd = log_listener(local_ip, LOG_PORT);
        FD_SET(pko_srv_fd, &master_set);
        FD_SET(log_fd, &master_set);
        ret = batch_io_loop(&master_set, log_fd, 0, 0);
    } else {
        ret = pko_dumpmemory_req(sock, file, offset, size);
		if (ret < 0) {
			printf("Sending execiop request failed\n");
		}
	}
    return 0;
}
