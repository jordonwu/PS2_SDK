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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <usb.h>
#include <syslog.h>

#include "packet.h"
#include "pl2301.h"
#include "common.h"
#include "rl_common.h"
#include "list.h"
#include "naplink.h"

/* #define DEBUG */
#define MAX_PACKET 8192

uid_t superuser;
uid_t loseruser;

int commandWaiting = 0;
char *commandBuffer = 0;
int benchmarking = 0;
char msg[MAX_PACKET];
static llist path_list;
static unsigned int counter;
static unsigned int processing = 0;
llist np_get_path(void);
usb_dev_handle * pl_hnd;
usb_dev_handle * get_handle(void);

/* get return value from target */
int get_return(usb_dev_handle *hnd, void *data)
{
    packet_header_t ph;
    return_data_t *pb = 0;
    int rv;

    recv_packet(hnd, &ph, (void *)&pb); 

    if (ph.type != PACKET_RETURN) {
        printf("Was expecting PACKET_RETURN, but got 0x%x\n", ph.type);
        return -1;
    }

    if ((ph.size > 4)&&(data))
        memcpy(data, pb->data, ph.size - 4);

    rv = pb->rv;

    if (pb)
    {
        free(pb);
    }

    return rv;
}

/* quit */
int
do_quit(void)
{
    usb_dev_handle *hnd = pl_hnd;
    packet_header_t ph;

    ph.type = PACKET_QUIT;
    ph.size = 0;

    send_packet(hnd, &ph, NULL);
    return get_return(hnd, NULL);
}

void
set_command(int c) {
    commandWaiting = c;
    return;
}

void
set_cmdbuf(char *cmd) {
    commandBuffer = cmd;
    return;
}

/* reset target */
int do_reset()
{
    usb_dev_handle *hnd = pl_hnd;
    packet_header_t ph;

    ph.type = PACKET_RESET;
    ph.size = 0;
    send_packet(hnd, &ph, NULL);
    return get_return(hnd, NULL);
}

/* execute ee elf on target */
int do_execps2(char *filename)
{
    usb_dev_handle *hnd = pl_hnd;
    packet_header_t ph;

    if (filename)
    {
        ph.type = PACKET_EXECPS2;
        ph.size = strlen(filename) + 1;
        send_packet(hnd, &ph, filename);
        return get_return(hnd, NULL);
    } else {
        return -1;
    }
}

/* execute iop irx on target */
int do_execiop(char *filename)
{
    usb_dev_handle *hnd = pl_hnd;
    packet_header_t ph;

    if (filename)
    {
        ph.type = PACKET_EXECIOP;
        ph.size = strlen(filename) + 1;
        send_packet(hnd, &ph, filename);
        return get_return(hnd, NULL);
    } else {
        return -1;
    }
}

/* send return value packet to target */
int do_return(usb_dev_handle *hnd, int rv, void *data, int count)
{
    packet_header_t ph;
    return_data_t *pb;

    if (count < 0)
        count = 0;

    pb = malloc(4 + count);

    ph.type = PACKET_RETURN;
    ph.size = 4 + count;

    pb->rv = rv;
    if (count) {
        memcpy(pb->data, data, count);
    }
    send_packet(hnd, &ph, pb);
}

/* perform a transfer rate test */
int do_benchmark(usb_dev_handle *hnd)
{
    int i = 0;
    packet_header_t ph;
    struct timeval start, end;
    float bps;

    ph.type = PACKET_BENCHMARK;
    ph.size = 0;

    send_packet(hnd, &ph, NULL);

    printf("benchmarking...");
    fflush(stdout);

    benchmarking = 1;

    gettimeofday(&start, NULL);

    while(i != PACKET_RETURN)
        i = recv_and_process_packet(hnd);

    gettimeofday(&end, NULL);

    benchmarking = 0;

    /* test is 32 reads of 64kbyte plus 32 writes of 64kbyte */
    bps = (65536 * 64) / ((end.tv_sec + end.tv_usec / 1000000.0) -(start.tv_sec + start.tv_usec / 1000000.0));

    printf(" %.0f bytes/second average\n", bps);
}

/* receive and process a packet from target */
int recv_and_process_packet(usb_dev_handle *hnd)
{
    char *buffer = 0;
    char path[MAXPATHLEN];
    llist path_list;
    packet_header_t packet;
    int rv;
    char *buffer2;
    int flags = 0;

    /* receive packet */
    recv_packet(hnd, &packet, (void *)&buffer);

    /* process packet */
    switch(packet.type) {
        case PACKET_OPEN:
            processing = 1;
            np_set_counter(0);
            if (DEBUG){
                printf("PACKET_OPEN\n");
                printf("pathname = %s\n", ((open_data_t *)buffer)->pathname);
                printf("ps2 flags = %d\n", ((open_data_t *)buffer)->flags);
            }
            if (!(strcmp( ((open_data_t *)buffer)->pathname, "/dev/tty"))) {
                rv = 1;
            } else {
                if (((open_data_t *)buffer)->flags & 1)
                    flags |= O_RDONLY;
                if (((open_data_t *)buffer)->flags & 2)
                    flags |= O_WRONLY;
                if (((open_data_t *)buffer)->flags & 0x100)
                    flags |= O_APPEND;
                if (((open_data_t *)buffer)->flags & 0x200)
                    flags |= O_CREAT;
                if (((open_data_t *)buffer)->flags & 0x400)
                    flags |= O_TRUNC;
                if (DEBUG) {
                    printf("pc flags = %d\n", flags);
                }
                if ((rv = open(((open_data_t *)buffer)->pathname, flags)) == -1) {
                    path_list = np_get_path();
                    while(path_list) {
                        strcpy(path, path_list->dir);
                        strcat(path, "/");
                        strcat(path, ((open_data_t *)buffer)->pathname); 
                        if ((rv = open(path, flags)) != -1) {
                            break;
                        }
                        path_list = path_list->next;
                    }
                }
            }
            do_return(hnd, rv, NULL, 0);
            break;
        case PACKET_CLOSE:
            processing = 0;
            np_set_counter(0);
            if(DEBUG){
                printf("PACKET_CLOSE\n");
                printf("fd = %d\n", ((close_data_t *)buffer)->fd);
            }
            if ( ((close_data_t *)buffer)->fd <= 2)
                rv = 0;
            else
                rv = close ( ((close_data_t *)buffer)->fd );
            do_return(hnd, rv, NULL, 0);
            break;
        case PACKET_READ:
            np_set_counter(0);
            if (DEBUG){
                printf("PACKET_READ\n");
                printf("fd = %d\n", ((read_write_data_t *)buffer)->fd);
                printf("count = %d\n", ((read_write_data_t *)buffer)->count);
            }
            buffer2 = malloc(((read_write_data_t *)buffer)->count);
            if (!benchmarking)
                rv = read( ((read_write_data_t *)buffer)->fd, buffer2, ((read_write_data_t *)buffer)->count );
            else
                rv = ((read_write_data_t *)buffer)->count;
            do_return(hnd, rv, buffer2, rv );
            free(buffer2);
            break;
        case PACKET_WRITE:
            np_set_counter(0);
            if(DEBUG){
                printf("PACKET_WRITE\n");
                printf("fd = %d\n", ((read_write_data_t *)buffer)->fd);
                printf("count = %d\n", ((read_write_data_t *)buffer)->count);
            }
            if (!benchmarking)
                rv = write( ((read_write_data_t *)buffer)->fd, ((read_write_data_t *)buffer)->data, ((read_write_data_t *)buffer)->count );
            else
                rv = ((read_write_data_t *)buffer)->count;
            do_return(hnd, rv, NULL, 0);
            break;
        case PACKET_LSEEK:
            np_set_counter(0);
            if(DEBUG){
                printf("PACKET_LSEEK\n");
                printf("fd = %d\n", ((lseek_data_t *)buffer)->fd);
                printf("offset = %d\n", ((lseek_data_t *)buffer)->offset);
                printf("whence = %d\n", ((lseek_data_t *)buffer)->whence);
            }
            rv = lseek( ((lseek_data_t *)buffer)->fd, ((lseek_data_t *)buffer)->offset, ((lseek_data_t *)buffer)->whence );
            do_return(hnd, rv, NULL, 0);
            break;
        case PACKET_WAZZUP:
            /* if(DEBUG) { */
            /*     printf("PACKET_WAZZUP\n"); */
            /*     printf("commandWaiting = %x\n", commandWaiting); */
            /* } */
            if (!processing) {
                np_update_counter();
            }
            switch (commandWaiting) {
                case 0:
                    rv = 0;
                    do_return(hnd, rv, NULL, 0);
                    break;
                case PACKET_RESET:
                    rv = do_reset(hnd);
                    printf("rv = %d\n", rv);
                    break;
                case PACKET_EXECPS2:
                    rv = do_execps2(commandBuffer);
                    printf("rv = %d\n", rv);
                    break;
                case PACKET_EXECIOP:
                    rv = do_execiop(commandBuffer);
                    printf("rv = %d\n", rv);
                    break;
                case PACKET_QUIT:
                    rv = do_quit();
                    printf("rv = %d\n", rv);
                    break;
                case PACKET_BENCHMARK:
                    do_benchmark(hnd);
                    break;
                default:
                    break;
            }
            commandWaiting = 0;
            break;
        case PACKET_RETURN:
            break;
        default:
            printf("BAD PACKET 0x%x\n",packet.type);
            break;
    }

    /* free buffer */
    if (buffer) {
        free(buffer);
    }

    return packet.type;
}

void
naplink_check(void) {
    usb_dev_handle *hnd = pl_hnd;
    if (CHECK_QLF(hnd, PEER_E) == 0) {
        printf("peer went away\n");

        /* seteuid(superuser); */

        usb_reset(hnd);
        usb_set_configuration(hnd, 1);
        usb_claim_interface(hnd, 0);

        /* seteuid(loseruser); */

        /* handshake */
        CLEAR_QLF(hnd, TX_REQ);
        CLEAR_QLF(hnd, TX_C);
        SET_QLF(hnd, PEER_E);

        printf("waiting for peer\n");
        while(CHECK_QLF(hnd, PEER_E) == 0);
        printf("peer is alive\n");
    }	
}

int
naplink_close(void) {
    CLEAR_QLF(pl_hnd, PEER_E);
    usb_release_interface(pl_hnd, 0);
    usb_close(pl_hnd);
    return(0);
}

void
naplink_debug(int l) {
    DEBUG = l;
}

void
naplink_process(void) {
    usb_dev_handle *hnd = pl_hnd;
    if (CHECK_QLF(hnd, TX_REQ)) {
        recv_and_process_packet(hnd);
    }
}

int
initialize_naplink(void) {
    struct usb_bus *bus;
    struct usb_device *dev;
    struct usb_bus *pl_bus = NULL;
    struct usb_device *pl_dev = NULL;
    int fd;
    int res = 99;

    usb_init();
    usb_find_busses();
    usb_find_devices();

    if (!usb_busses) {
        fprintf(stderr, "USB initialization failed\n");
    }

    for (bus = usb_busses; bus; bus = bus->next) {
        for (dev = bus->devices; dev; dev = dev->next) {
            printf("Found device %04X/%04X\n", 
                dev->descriptor.idVendor,
                dev->descriptor.idProduct);
            if (dev->descriptor.idVendor == PROLIFIC_VENDOR_ID) {
                switch (dev->descriptor.idProduct) {
                    case 0:
                        printf("Found a PL2301\n");
                        pl_bus = bus;
                        pl_dev = dev;
                        break;
                    case 1:
                        printf("Found a PL2302\n");
                        pl_bus = bus;
                        pl_dev = dev;
                        break;
                    default:
                        printf("Found an unknown Prolific device (id %d)\n", dev->descriptor.idProduct);
                        break;
                }
            }
        }
    }

    if ((!pl_bus)||(!pl_dev)) {
        printf("No supported devices detected.\n");
        return -1;
    }

    /* seteuid(superuser); */

    /* open device */
    pl_hnd = usb_open(pl_dev);

    usb_reset(pl_hnd);

    usb_set_configuration(pl_hnd, 1);
    usb_claim_interface(pl_hnd, 0);

    /* seteuid(loseruser); */

    /* handshake */
    CLEAR_QLF(pl_hnd, TX_REQ);
    CLEAR_QLF(pl_hnd, TX_C);
    SET_QLF(pl_hnd, PEER_E);

    printf("waiting for peer\n");
    while(CHECK_QLF(pl_hnd, PEER_E) == 0);
    printf("peer is alive\n");

    return 0;
}

void
np_set_path(llist ptr) {
    path_list = ptr;
}

void
np_update_counter(void) {
    counter = counter + 1;
}

void
np_set_counter(unsigned int n) {
    counter = n;
}

unsigned int
np_get_counter(void) {
    return counter;
}

llist
np_get_path(void) {
    return path_list;
}

void
np_set_processing(unsigned int n) {
    processing = n;
}
