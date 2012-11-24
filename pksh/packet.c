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

#include <usb.h>

#include "packet.h"
#include "pl2301.h"

extern uid_t superuser;
extern uid_t loseruser;

/* #define DEBUG */

/* receive a packet */
int recv_packet(usb_dev_handle *hnd, packet_header_t *ph, void **pb)
{
    if(DEBUG) {
        printf("Waiting for PS2 to set TX_REQ...\n");
    }

    /* wait for TX_REQ */
    while(CHECK_QLF(hnd, TX_REQ) == 0);

    /* clear TX_REQ */
    CLEAR_QLF(hnd, TX_REQ);

    if(DEBUG) {
        printf("Receiving packet from PS2...\n");            
    }

    /* seteuid(superuser); */
    /* receive packet header */
    usb_bulk_read(hnd, 0x83, (char *)ph, 8, 10000);

    /* receive packet body */
    if (ph->size) {
	*pb = malloc(ph->size);
	usb_bulk_read(hnd, 0x83, *pb, ph->size, 10000);
    }	
    /* seteuid(loseruser); */

    if(DEBUG){
        printf("Waiting for TX_C...\n");
    }

    /* wait for TX_C */
    while(CHECK_QLF(hnd, TX_C) == 0);

    /* clear TX_C */
    CLEAR_QLF(hnd, TX_C);

    if(DEBUG){
        printf("Packet received from PS2!\n");
    }
}

/* send a packet */
int send_packet(usb_dev_handle *hnd, packet_header_t *ph, void *pb)
{
    if(DEBUG){
        printf("Waiting for TX_RDY...\n");
    }

    /* wait for TX_RDY */
    while(CHECK_QLF(hnd, TX_RDY) == 0);

    /* set TX_REQ */
    SET_QLF(hnd, TX_REQ);

    if(DEBUG){
        printf("Waiting for TX_REQ to clear...\n");
    }

    /* wait for peer to clear TX_REQ */
    while(CHECK_QLF(hnd, TX_REQ));

    if(DEBUG){
        printf("Sending packet to PS2...\n");                
    }

    /* seteuid(superuser); */
    /* send bulk */
    usb_bulk_write(hnd, 0x02, (char *)ph, 8, 10000);

    if (ph->size)
	usb_bulk_write(hnd, 0x02, pb, ph->size, 10000);
    /* seteuid(loseruser); */

    /* set TX_C */
    SET_QLF(hnd, TX_C);

    if(DEBUG){
        printf("Packet sent to PS2!\n");
    }
}
