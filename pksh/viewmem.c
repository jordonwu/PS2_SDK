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
    int sock, ret, local, offset, size, width, dump_as_float, i, j;
    unsigned char file[MAX_PATH];
    unsigned char tmpfile[MAX_PATH];
	struct stat st;
	char ch;
	unsigned char data[4];
	char *end_ptr;
	FILE *fp = NULL;
    memset(file, 0, MAX_PATH);
    memset(data, 0, 4);
    memset(tmpfile, 0, MAX_PATH);
    memset(dst_ip, 0, 16);
	width = 4;
	dump_as_float = 0;

    if (argc < 2) {
        printf("Usage:\n"
               " %s offset size\n", argv[0]);
		printf(" %s -d <ps2link ip> -q|-w -f offset size\n", argv[0]);
        return 0;
    }

	while((ch = getopt(argc, argv, "h:fwq")) != -1 ) {
		switch((char)ch) {
			case 'h':
				strncpy(dst_ip, optarg, 16);
				break;
			case 'f':
				dump_as_float = 1;
				break;
			case 'w':
				width = 4;
				break;
			case 'q':
				width = 16;
				break;
			default:
				/* usage(); */
				break;
		}
	}

	tmpnam(tmpfile);
    offset = (unsigned int)strtol(argv[argc-2], &end_ptr, 0);
	if ( argv[argc-2] == end_ptr ) {
		printf("offset is not a valid number\n");
		/* usage(); */
		return 0;
	}
    size = (unsigned int)strtol(argv[argc-1], &end_ptr, 0);
	if ( argv[argc-1] == end_ptr ) {
		printf("size is not a valid number\n");
		/* usage(); */
		return 0;
	}

    // check if pksh is running, if so use it
    if ( (ps2link_open(&sock, local_ip)) < 0 ) {
       // do we have an alternative ip ?
		close(cmd_fd);
        if ( ps2link_open(&cmd_fd, dst_ip) < 0 ) {
            printf("Unable to connect to ps2link server\n");
            return -1;
        }
        printf("Connected to %s ps2link server\n", dst_ip);
        local = 0;
    } else {
        local = 1;
        printf("Connected to local ps2link server ( pksh )\n");
    }

	strcat(file, "host:");
	strcat(file, tmpfile);

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
        if (ret < 0) {
            printf("Sending dumpreg request failed\n");
        }
    } else {
        ret = pko_dumpmemory_req(cmd_fd, file, offset, size);
        if (ret < 0) {
            printf("Sending dumpmemory request failed\n");
        }
    }

	// ok lets dump mem
	while(1) {
		ret = stat(tmpfile, &st);
		if ( ret != 0 ) {
			continue;
		}
		if (st.st_size == size) {
			break;
		}
#ifdef __WIN32__
		Sleep(1000);
#else
		usleep(1000);
#endif
	}

	fp = fopen(tmpfile, "rb");
	if ( fp == NULL ) {
		perror("");
		return 0;
	}
	for(i = 0; i < size; i = i + width) {
		for(j = 0; j < width/4; j++) {
			if ( dump_as_float ) {
				fread(&data, 4, 1, fp);
				ret = (data[3]<<24)+(data[2]<<16)+
						(data[1]<<8)+(data[0]<<0);
				printf("%20.10f ", *((float *)&ret));
			} else {
				fread(&data, 4, 1, fp);
				printf("0x%08x ", (unsigned int)(
						(data[3]<<24)+(data[2]<<16)+
						(data[1]<<8)+(data[0]<<0)
						));
			}
		}
		printf("\n");
	}
	fflush(stdout);
	fclose(fp);
	unlink(tmpfile);
    return 0;
}
