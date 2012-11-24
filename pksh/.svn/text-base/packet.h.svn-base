/*
 * NapLink Linux Client v1.0.0 by AndrewK of Napalm
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

#ifndef __PACKET_H__
#define __PACKET_H__

typedef struct {
    unsigned int size;
    unsigned int type;
} packet_header_t;

typedef struct {
    int flags;
    char pathname[256];
} open_data_t;

typedef struct {
    int fd;
} close_data_t;

typedef struct {
    int fd;
    int count;
    char data[1];
} read_write_data_t;

typedef struct { 
    int fd;
    int offset;
    int whence;
} lseek_data_t;

typedef struct {
    int rv;
    char data[1];
} return_data_t;

/* pc to ps2 */
#define PACKET_RESET      0x004d704e
#define PACKET_EXECPS2    0x014d704e
#define PACKET_EXECIOP    0x024d704e
#define PACKET_QUIT       0x034d704e
#define PACKET_BENCHMARK  0x044d704e

/* ps2 to pc */
#define PACKET_OPEN       0x104d704e
#define PACKET_CLOSE      0x114d704e
#define PACKET_READ       0x124d704e
#define PACKET_WRITE      0x134d704e
#define PACKET_LSEEK      0x144d704e
#define PACKET_WAZZUP     0x154d704e

/* both */

#define PACKET_RETURN     0x204d704e

int recv_packet(usb_dev_handle *hnd, packet_header_t *ph, void **pb);
int send_packet(usb_dev_handle *hnd, packet_header_t *ph, void *pb);

static int DEBUG = 0;

#endif
