/*
 * Copyright (c) Tord Lindström and Khaled Daham
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

char dst_ip[16];
char *local_ip = "127.0.0.1";

int
main(int argc, char **argv)
{
    int sock, ret, local;
    char ch;
    unsigned char file[MAX_PATH];
    memset(file, 0, MAX_PATH);

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
    if ( (ps2link_open(&sock, local_ip)) < 0 ) {
        // do we have an alternative ip ?
		close(sock);
        if ( ps2link_open(&sock, dst_ip) < 0 ) {
            printf("Unable to connect to ps2link server\n");
            return -1;
        }
        printf("Connected to %s ps2link server\n", dst_ip);
        local = 0;
    } else {
        local = 1;
        printf("Connected to local ps2link server ( pksh )\n");
    }

    if (!local) {
        pko_cmd_con(dst_ip, CMD_PORT);
        ret = pko_reset_req(cmd_fd);
        if (ret < 0) {
            printf("reset req failed\n");
        }
    } else {
        ret = pko_reset_req(sock);
        if (ret < 0) {
            printf("reset req failed\n");
        }
    }
    return 0;
}
