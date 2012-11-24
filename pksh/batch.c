/* -*- C -*-
 * (C) 2004 by Khaled Daham
 */
#include "linkproto_core.h"
#include "linkproto_stub.h"
#include "batch.h"

static int service_close = 1;

int
batch_io_loop(fd_set *master_set, int maxfd, int nservice, int timeout) {
    int ret, doloop;
    fd_set readset;
    doloop = 1;

    while(doloop) {
        readset = *master_set;
        ret = select(maxfd+1, &readset, NULL, NULL, NULL);
        if ( ret < 0 ) {
            perror("select");
            return 0;
        } else if (ret == 0) {
        } else {
            if ( FD_ISSET(log_fd, &readset) ) {
                batch_log_read(log_fd);
            } else if ( FD_ISSET(pko_srv_fd, &readset) ) {
                batch_io_read(pko_srv_fd);
			}
        }
		if ( (nservice == 0) && (service_close == 0)) {
			break;
		}
    }
    return 0;
}

int
batch_io_read(int fd) {
    int length = 0;
    int ret = 0;
    unsigned int cmd;
    unsigned short hlen;
    pkt_hdr *header;
    
    length = pko_recv_bytes(fd, &recv_packet[0], sizeof(pkt_hdr));
    if ( length < 0 ) {
        return length;
    } else if ( length == 0 ) {
    } else {
        header = (pkt_hdr *)&recv_packet[0];
        cmd = ntohl(header->cmd);
        hlen = ntohs(header->len);
        ret = pko_recv_bytes(fd, &recv_packet[length],
            hlen - sizeof(pkt_hdr));

        switch (cmd) {
            case OPEN_CMD:
                pko_open_file(&recv_packet[0]);
                break;
            case CLOSE_CMD:
                pko_close_file(&recv_packet[0]);
				service_close = 0;
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
            default:
                printf(" unknown fileio command received (%x)\n", header->cmd);
                break;
        }
    }
    return 0;
}

int
batch_log_read(int fd) {
    int ret;
    int size = sizeof(struct sockaddr_in);
    static char buf[1024];
    struct sockaddr_in dest_addr;
    memset(buf, 0x0, sizeof(buf));
    memset(&(dest_addr), '\0', size);
    ret = recvfrom(
        fd, buf, sizeof(buf), 0,
        (struct sockaddr *)&dest_addr, &size
        );
    if (ret == -1) {
        perror("read");
    }
    fprintf(stdout, "%s", buf);
    fflush(stdout);
    return ret;
}
