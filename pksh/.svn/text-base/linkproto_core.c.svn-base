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

#include <string.h>
#include "linkproto_core.h"

static llist path_list;
static int DEBUG;

void
pko_set_debug(int level) {
    DEBUG = level;
    return;
}

int
pko_debug(void) {
    return DEBUG;
}

void
pko_set_root(char *p) {
    /* printf("new root = %s\n", p); */
    /* strcpy(rootdir, p); */
    return;
}

int
ps2netfs_open(char *dst_ip, int port, int timeout) {
    return cmd_listener(dst_ip, port, timeout);
}

int
log_listener(char *src_ip, int port) {
    int fd;
    int server_addr_size = sizeof(struct sockaddr_in);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(src_ip);
    memset(&(server_addr.sin_zero), '\0', 8);
    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("UDP log socket"); 
    }
    if((bind(fd, (struct sockaddr *)&server_addr, server_addr_size)) == -1) {
        perror("UDP log bind");
    }
    return fd;
}

int
cmd_listener(char *dst_ip, int port, int timeout) {
    int addr_size = sizeof(struct sockaddr_in);
    int len, flags, ret, error;
    struct sockaddr_in addr;
    struct timeval time;
    fd_set rset, wset;
    int fd;
    int yes = 1;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(dst_ip);
    memset(&(addr.sin_zero), '\0', 8);

    if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
        perror("socket");
        return -1;
    }
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
    }
    error = 0;
    if(fcntl(fd, F_SETFL, flags|O_NONBLOCK) < 0) {
        perror("fcntl");
    }
    if((ret = connect(fd, (struct sockaddr *)&addr, addr_size)) < 0) {
        if (errno != EINPROGRESS) {
            return(-1);
        }
    }
    if(ret == 0) {
        goto done;
    }
    FD_ZERO(&rset);
    FD_SET(fd, &rset);
    wset = rset;

    time.tv_usec = 0;
    time.tv_sec = timeout;
    if((ret = select(fd+1, &rset, &wset, 0, &time)) == 0) {
        close(fd);
        errno = ETIMEDOUT;
        return(-1);
    }

    len = sizeof(error);

    if(FD_ISSET(fd, &rset) || FD_ISSET(fd, &wset)) {
        if(getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
            return(-1);
        }
    } else {
        perror("");
    }

done:
    fcntl(fd, F_SETFL, flags);
    if(error) {
        close(fd);
        errno = error;
        return(-1);
    }
    return fd;
}

int
pko_srv_setup(char *src_ip, int port) {
    int flags, ret, fd;
    struct sockaddr_in addr;
    int backlog = 10;
    int yes = 1;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(src_ip);
    memset(&(addr.sin_zero), '\0', 8);

    if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
        perror("socket");
        return -1;
    }
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        return -1;
    }
    if(fcntl(fd, F_SETFL, flags|O_NONBLOCK) < 0) {
        perror("fcntl");
        return -1;
    }
    if((bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))) == -1){
        perror("bind");
        return -1;
    }
    if((ret = listen(fd, backlog)) != 0) {
        perror("listen");
        return -1;
    }
    return fd;
}

int
pko_cmd_con(char *ip, int port) {
    int ret;
    struct sockaddr_in server_addr;

    bzero((void *)&server_addr, sizeof(server_addr));
    inet_aton(ip, &(server_addr.sin_addr));

    // Connect to server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if ((cmd_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    ret = connect(cmd_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        perror("Connect");
        return -1;
    }
    return cmd_fd;
}

int pko_send_bytes(int s, char *buf, int bytes) {
    int left;
    int len;

    left = bytes;

    while (left > 0) {
        len = send(s, &buf[bytes - left], left, 0);
        if (len < 0) {
            if ((errno == EPIPE) || (errno == ECONNRESET) || 
                (errno == ECONNABORTED)) {
                return len;
            }
            // Other error?
            continue;
        }
        if (len == 0) {
            // EOF
            return 0;
        }
        left -= len;
    }
    return bytes;
}

int
pko_recv_bytes(int s, char *buf, int bytes) {
    int left;
    int len;

    left = bytes;
    while (left > 0) {
        len = recv(s, &buf[bytes - left], left, 0);
        if (len < 0) {
            if ((errno == EPIPE) || (errno == ECONNRESET) || 
                (errno == ECONNABORTED)) {
                return len;
            }
            if ( DEBUG ) { 
                perror(" recv");
            }
            continue;
        }
        // EOF
        if (len == 0) {
            return 0;
        }
        
        left -= len;
    }
    return bytes;
}

/*
 * File IO commands
 */
int
pko_open_file(char *buf) {
    unsigned short len;
    int retval;
    int flags;
    int mode = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH;
    char path[MAXPATHLEN];
    pkt_file_req *pkt;
    pkt_file_rly *reply;
    llist path_list;
    pkt = (pkt_file_req *)buf;
    flags = pko_fix_flags(ntohl(pkt->flags));
    len = sizeof(pkt_file_rly);

    reply = (pkt_file_rly *)&send_packet[0];
    reply->cmd = htonl(OPEN_RLY);
    reply->len = htons(len);

    pko_fix_path(pkt->path);

    if (DEBUG) {
        printf("open file %s (%x)\n", pkt->path, flags);
    }

    /* should probably check if we get CREAT or READ here */
    if ((retval = open(pkt->path, flags, mode)) == -1) {
        path_list = pko_get_path();
        while(path_list) {
            strcpy(path, path_list->dir);
            strcat(path, "/");
            strcat(path, pkt->path); 
            if ((retval = open(path, flags, mode)) != -1) {
                break;
            }
            path_list = path_list->next;
        }
    }

    reply->retval = htonl(retval);

    if (DEBUG) {
        printf("got fd %d\n", retval);
    }

    send(pko_srv_fd, reply, len, 0);
    return 0;
}

int
pko_open_dir(char *buf) {
    unsigned short len;
    DIR *dirp;
    pkt_file_req *pkt;
    pkt_file_rly *reply;
    pkt = (pkt_file_req *)buf;
    len = sizeof(pkt_file_rly);

    reply = (pkt_file_rly *)&send_packet[0];
    reply->cmd = htonl(DOPEN_RLY);
    reply->len = htons(len);

    pko_fix_path(pkt->path);
    if (DEBUG) {
        printf("open dir %s\n", pkt->path);
    }

    if ((dirp = opendir(pkt->path)) == NULL) {
        return -1;
    }

    reply->retval = htonl((int)dirp);

    if (DEBUG) {
        printf("got fd %d\n", (unsigned int)ntohl(reply->retval));
    }

    send(pko_srv_fd, reply, len, 0);
    return 0;
}

int
pko_read_dir(char *buf) {
    struct stat st;
    struct tm loctime;
    struct dirent *dirp;
    pkt_read_req *pkt;
    pkt_dread_rly *reply;

    pkt = (pkt_read_req *)buf;
    reply = (pkt_dread_rly *)&send_packet[0];
    reply->cmd = htonl(DREAD_RLY);
    reply->len = htons(sizeof(pkt_dread_rly));
    reply->retval = htonl((int)dirp = readdir((DIR *)ntohl(pkt->fd)));
    if ( dirp != 0 ) {
        stat(dirp->d_name, &st);

        // mode
        reply->mode = (st.st_mode & 0x07);
        if (S_ISDIR(st.st_mode)) { reply->mode |= 0x20; }
        if (S_ISLNK(st.st_mode)) { reply->mode |= 0x08; }
        if (S_ISREG(st.st_mode)) { reply->mode |= 0x10; }
        reply->mode = htonl(reply->mode);
        
        // add attributes
        reply->attr = htonl(0);

        // size
        reply->size = htonl(st.st_size);

        // add time
        if (localtime_r(&(st.st_ctime), &loctime)) {
            reply->ctime[6] = loctime.tm_year;
            reply->ctime[5] = loctime.tm_mon + 1;
            reply->ctime[4] = loctime.tm_mday;
            reply->ctime[3] = loctime.tm_hour;
            reply->ctime[2] = loctime.tm_min;
            reply->ctime[1] = loctime.tm_sec;
        }

        if (localtime_r(&(st.st_atime), &loctime)) {
            reply->atime[6] = loctime.tm_year;
            reply->atime[5] = loctime.tm_mon + 1;
            reply->atime[4] = loctime.tm_mday;
            reply->atime[3] = loctime.tm_hour;
            reply->atime[2] = loctime.tm_min;
            reply->atime[1] = loctime.tm_sec;
        }

        if (localtime_r(&(st.st_mtime), &loctime)) {
            reply->mtime[6] = loctime.tm_year;
            reply->mtime[5] = loctime.tm_mon + 1;
            reply->mtime[4] = loctime.tm_mday;
            reply->mtime[3] = loctime.tm_hour;
            reply->mtime[2] = loctime.tm_min;
            reply->mtime[1] = loctime.tm_sec;
        }
        reply->hisize = htonl(0);
        strncpy(reply->path, dirp->d_name, 256);
    }

    send(pko_srv_fd, reply, sizeof(pkt_dread_rly), 0);
    return 0;
}

int
pko_close_dir(char *buf) {
    int retval;
    int fd;
    pkt_close_req *pkt;
    pkt_file_rly *reply;

    pkt = (pkt_close_req *)buf;
    fd = ntohl(pkt->fd);

    if (DEBUG)
        printf("close filedesc %d\n", fd);

    retval = closedir((DIR *)fd);

    reply = (pkt_file_rly *)&send_packet[0];
    reply->cmd = htonl(DCLOSE_RLY);
    reply->len = htons(sizeof(pkt_file_rly));
    reply->retval = htonl(retval);

    send(pko_srv_fd, reply, sizeof(pkt_file_rly), 0);
    return 0;
}

int
pko_close_file(char *buf)
{
    unsigned short len;
    int retval;
    int fd;
    pkt_close_req *pkt;
    pkt_file_rly *reply;

    pkt = (pkt_close_req *)buf;
    len = sizeof(pkt_file_rly);
    fd = ntohl(pkt->fd);

    if (DEBUG)
        printf("close filedesc %d\n", fd);

    retval = close(fd);

    reply = (pkt_file_rly *)&send_packet[0];
    reply->cmd = htonl(CLOSE_RLY);
    reply->len = htons(len);
    reply->retval = htonl(retval);

    send(pko_srv_fd, reply, sizeof(pkt_file_rly), 0);

    return 0;
}

int
pko_read_file(char *buf)
{
    unsigned short len;
    int retval, nbytes, fd;
    int readbytes;
    pkt_read_req *pkt;
    pkt_read_rly *reply;
    char *filebuf;
    int ret;

    pkt = (pkt_read_req *)buf;
    len = sizeof(pkt_read_rly);
    nbytes = ntohl(pkt->nbytes);
    fd = ntohl(pkt->fd);
    retval = ntohl(pkt->fd);

    if (DEBUG)
        printf("read %d bytes from file desc %d\n", nbytes, fd);

    reply = (pkt_read_rly *)&send_packet[0];
    reply->cmd = htonl(READ_RLY);
    reply->len = htons(len);
    reply->retval = htonl(retval);

#if 1
    {
        filebuf = (char *)malloc(nbytes);
        if (filebuf == NULL) {
            printf("Ooops, read_file() could not alloc mem!\n"
                   "  wanted %d bytes\n", nbytes);
            return -1;
        }
        readbytes = read(fd, filebuf, nbytes);
        if (DEBUG) {
            printf("read %d bytes\n", readbytes);
        }
    }
    reply->nbytes = htonl(readbytes);
    send(pko_srv_fd, reply, sizeof(pkt_read_rly), 0);
    ret = pko_send_bytes(pko_srv_fd, filebuf, readbytes);
    if (ret != readbytes) {
        perror("");
        printf("sent %d, wanted to send %d\n", ret, readbytes);
    }
    free(filebuf);
#else
    if (pkt->nbytes > MAX_READ_SEGMENT) {
        if (DEBUG)
            printf("Gah!! Trying to read more than MAX_SEGMENT_SIZE bytes!!!\n");
        reply->nbytes = -1;
    }
    else {
        reply->nbytes = read(pkt->fd, &send_packet[sizeof(pkt_read_rly)],
                             pkt->nbytes);
        if (DEBUG)
            printf("read %d bytes\n", reply->nbytes);
    }

    if (reply->nbytes < 0) {
        // Error
        perror("read file");
        send(pko_srv_fd, reply, sizeof(pkt_read_rly), 0);
    }
    else {
        send(pko_srv_fd, reply, sizeof(pkt_read_rly) + reply->nbytes, 0);
    }        
#endif

    return 0;
}

int
pko_write_file(char *buf) {
    int fd;
    int nbytes;
    int written;
    int bytes_left;

    pkt_write_req *pkt;
    pkt_file_rly *reply;
    char *databuf;

    pkt = (pkt_write_req *)buf;
    fd = ntohl(pkt->fd);
    nbytes = ntohl(pkt->nbytes);

    if (DEBUG)
        printf("write %d bytes to filedesc %d\n", nbytes, fd);

    databuf = &recv_packet[0];
    bytes_left = nbytes;
    do {
        int recvlen;
        int read_bytes;

        if (bytes_left > PACKET_MAXSIZE) {
            read_bytes = PACKET_MAXSIZE;
            if (DEBUG)
                printf("Dividing write into segments! (Should never happen again!!) (%d)\n", bytes_left);
        } else {
            read_bytes = bytes_left;
        }

        //        recvlen = recv(pko_srv_fd, databuf, read_bytes, 0);
        recvlen = pko_recv_bytes(pko_srv_fd, databuf, read_bytes);

        if (recvlen < 0) {
            // Error
            if (DEBUG)
                printf("pko_write_file: got error while reading\n");
            break;
        }
        if (recvlen == 0) {
            // EOF (pko_srv_fdet closed)
            if (DEBUG)
                printf("pko_write_file: got EOF while reading\n");
            break;
        }

        written = write(fd, databuf, recvlen);
        if (written < 0) {
            perror("pko_write_file (write)");
            break;
        }
        
        bytes_left -= written;

        if (written < recvlen) {
            // Not able to write it all??
            if (DEBUG)
                printf("pko_write_file (write): Could not write whole buffer!?\n");
            break;
        }

    } while (bytes_left > 0);

    reply = (pkt_file_rly *)&send_packet[0];
    reply->cmd = htonl(WRITE_RLY);
    reply->len = htons((unsigned short)sizeof(pkt_file_rly));
    reply->retval = htonl(nbytes - bytes_left);
    send(pko_srv_fd, reply, sizeof(pkt_file_rly), 0);
    return 0;
}

int
pko_lseek_file(char *buf) {
    unsigned short len;
    int retval;
    int fd, offset, whence;
    pkt_lseek_req *pkt;
    pkt_file_rly *reply;

    pkt = (pkt_lseek_req *)buf;
    len = sizeof(pkt_file_rly);
    fd = ntohl(pkt->fd);
    offset = ntohl(pkt->offset);
    whence = ntohl(pkt->whence);
    retval = lseek(fd, offset, whence);

    reply = (pkt_file_rly *)&send_packet[0];
    reply->cmd = htonl(LSEEK_RLY);
    reply->len = htons(len);
    reply->retval = htonl(retval);
    if (DEBUG) {
        printf("got fd %d\n", retval);
    }

    send(pko_srv_fd, reply, sizeof(pkt_file_rly), 0);
    return 0;
}

/*
 * Misc helpers
 */
void
pko_set_path(llist ptr) {
    path_list = ptr;
}

llist
pko_get_path(void) {
    return path_list;
}

void
pko_fix_path(char *a) {
    char c;
    if (a[0] == 0) {
        strcpy(a, ".");
    }
    /* Convert DOS-style '\' to '/' */
    while ((c = *a++) != '\0') {
        if(c == '\\') {
            *(a - 1) = '/';
        }
    }
}

int
pko_fix_flags(int flags) {
    int newflags = 0;
    if (flags & PS2_O_RDONLY) newflags |= O_RDONLY;
    if (flags & PS2_O_WRONLY) newflags |= O_WRONLY;
    //    if (flags & PS2_O_RDWR)   newflags |= O_RDWR;
    if (flags & PS2_O_NBLOCK) newflags |= O_NONBLOCK;
    if (flags & PS2_O_APPEND) newflags |= O_APPEND;
    if (flags & PS2_O_CREAT)  newflags |= O_CREAT;
    if (flags & PS2_O_TRUNC)  newflags |= O_TRUNC;

    // Hm, what to do if there's bits that are not handled above? 
    // Just OR them in for now..
    flags &= ~(PS2_O_RDWR | PS2_O_NBLOCK | PS2_O_APPEND | 
               PS2_O_CREAT | PS2_O_TRUNC);
    newflags |= flags;
    return newflags;
}
