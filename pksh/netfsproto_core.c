/* -*- C -*-
 * (C) 2004 by Adresd, <adresd_ps2dev@yahoo.com>
 * (C) 2004 by Khaled Daham
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <dirent.h>
#include <time.h>

#include "linkproto_core.h"
#include "netfsproto_core.h"

// ------------------------------------------------------------------
int
ps2netfs_accept_pkt(int sock, char *buf, int len, int pkt_type) {
    unsigned int length;
    pkt_hdr     *hdr;
    int hcmd;
    unsigned short hlen;

    length = pko_recv_bytes(sock, buf, sizeof(pkt_hdr));
    if (length <= 0) {
        printf("ps2netfs: accept_pkt recv error\n");
        return -1;
    }

    if (length < sizeof(pkt_hdr)) {
        printf("ps2netfs: XXX: did not receive a full header!!!! "
            "Fix this! (%d)\n", length);
    }

    hdr = (pkt_hdr *)buf;
    hcmd = ntohl(hdr->cmd);
    hlen = ntohs(hdr->len);

    if (hcmd != pkt_type) {
        close(ps2_netfs_fd);
        ps2_netfs_fd = -1;
        printf("ps2netfs: accept_pkt: Expected %x, got %x\n",
            pkt_type, hcmd);
        return 0;
    }

    if (hlen > len) {
        printf("ps2netfs: accept_pkt: hdr->len is too large!! "
            "(%d, can only receive %d)\n", hlen, len);
        return 0;
    }

    // get the actual packet data
    length = pko_recv_bytes(sock, buf + sizeof(pkt_hdr), 
        hlen - sizeof(pkt_hdr));

    if (length <= 0) {
        printf("ps2netfs: accept recv2 error!!\n");
        return 0;
    }

    if (length < (hlen - sizeof(pkt_hdr))) {
        printf("ps2netfs: Did not receive full packet!!! "
            "Fix this! (%d)\n", length);
    }
    return 1;
}

// ------------------------------------------------------------------
int
ps2netfs_req_open(char *path, int mode) {
    pkt_file_rly    *response;
    pkt_file_req    *request;
    request         = (pkt_file_req *)&send_packet[0];
    response        = (pkt_file_rly *)&recv_packet[0];

    // Build the request packet.
    request->cmd = htonl(PS2NETFS_OPEN_CMD);
    request->len = htons(sizeof(pkt_file_req));
    request->flags = htonl(mode);
    strncpy(request->path, path, MAX_PATH);

    // Send the request packet.
    send(ps2_netfs_fd, request, sizeof(pkt_file_req), 0);

    // now get the response
    if (!ps2netfs_accept_pkt(ps2_netfs_fd, (char *)response, 
            sizeof(pkt_file_rly), PS2NETFS_OPEN_RLY)) {
        return -1;
    }
    return ntohl(response->retval);
}

// ------------------------------------------------------------------
int
ps2netfs_req_close(int fd) {
    pkt_file_rly    *response;
    pkt_close_req   *request;
    request         = (pkt_close_req *)&send_packet[0];
    response        = (pkt_file_rly *)&recv_packet[0];

    // Build the request packet.
    request->cmd = htonl(PS2NETFS_CLOSE_CMD);
    request->len = htons(sizeof(pkt_close_req));
    request->fd = htonl(fd);

    // Send the request packet.
    send(ps2_netfs_fd, request, sizeof(pkt_close_req), 0);

    // now get the response
    if (!ps2netfs_accept_pkt(ps2_netfs_fd, (char *)response, 
            sizeof(pkt_file_rly), PS2NETFS_CLOSE_RLY)) {
        return -1;
    }
    return ntohl(response->retval);
}

// ------------------------------------------------------------------
int
ps2netfs_req_read(int fd, void *ptr, int size) {
    pkt_read_rly    *response;
    pkt_read_req    *request;
    response        = (pkt_read_rly *)&recv_packet[0];
    request         = (pkt_read_req *)&send_packet[0];

    // Build the request packet.
    request->cmd = htonl(PS2NETFS_READ_CMD);
    request->len = htons(sizeof(pkt_read_req));
    request->fd  = htonl(fd);
    request->nbytes = htonl(size);

    // Send the request packet.
    send(ps2_netfs_fd, request, sizeof(pkt_read_req), 0);

    // now get the response
    if (!ps2netfs_accept_pkt(ps2_netfs_fd, (char *)response, 
            sizeof(pkt_read_rly), PS2NETFS_READ_RLY)) {
        return -1;
    }

    if (ntohl(response->retval) > 0) {
        pko_recv_bytes(ps2_netfs_fd, ptr, ntohl(response->retval));
    }
    return ntohl(response->retval);
}

// ------------------------------------------------------------------
int
ps2netfs_req_write(int fd, void *ptr, int size) {
    int ret;
    pkt_file_rly    *response;
    pkt_write_req   *request;
    response        = (pkt_file_rly *)&recv_packet[0];
    request         = (pkt_write_req *)&send_packet[0];

    // Build the request packet.
    request->cmd = htonl(PS2NETFS_WRITE_CMD);
    request->len = htons(sizeof(pkt_write_req));
    request->fd  = htonl(fd);
    request->nbytes = htonl(size);

    // Send the request packet.
    ret = send(ps2_netfs_fd, request, sizeof(pkt_write_req), 0);
    if ( ret < 0 ) {
        printf("send write request failed.\n");
        return -1;
    }

    // Send the requested read data.
    ret = send(ps2_netfs_fd, ptr, size, 0);
    if ( ret < 0 ) {
        printf("send write data failed.\n");
        return -1;
    }

    // now get the response
    if (!ps2netfs_accept_pkt(ps2_netfs_fd, (char *)response,
            sizeof(pkt_file_rly), PS2NETFS_WRITE_RLY)) {
        return -1;
    }
    return ntohl(response->retval);
}

// ------------------------------------------------------------------
int
ps2netfs_req_lseek(int fd, int offset, int whence) {
    pkt_file_rly    *response;
    pkt_lseek_req   *request;
    response        = (pkt_file_rly *)&recv_packet[0];
    request         = (pkt_lseek_req *)&send_packet[0];

    // Build the request packet.
    request->cmd = htonl(PS2NETFS_LSEEK_CMD);
    request->len = htons(sizeof(pkt_lseek_req));
    request->fd     = htonl(fd);
    request->offset = htonl(offset);
    request->whence = htonl(whence);

    // Send the request packet.
    send(ps2_netfs_fd, request, sizeof(pkt_lseek_req), 0);

    // now get the response
    if (!ps2netfs_accept_pkt(ps2_netfs_fd, (char *)response, 
            sizeof(pkt_file_rly), PS2NETFS_LSEEK_RLY)) {
        return -1;
    }
    return ntohl(response->retval);
}

// ------------------------------------------------------------------
int
ps2netfs_req_dopen(char *path) {
    pkt_file_rly    *response;
    pkt_file_req    *request;
    response        = (pkt_file_rly *)&recv_packet[0];
    request         = (pkt_file_req *)&send_packet[0];

    // Build the request packet.
    request->cmd = htonl(PS2NETFS_DOPEN_CMD);
    request->len = htons(sizeof(pkt_file_req));
    strncpy(request->path, path, MAX_PATH);

    // Send the request packet.
    send(ps2_netfs_fd, request, sizeof(pkt_file_req), 0);

    // now get the response
    if (!ps2netfs_accept_pkt(ps2_netfs_fd, (char *)response, 
            sizeof(pkt_file_rly), PS2NETFS_DOPEN_RLY)) {
        return -1;
    }
    return ntohl(response->retval);
}

// ------------------------------------------------------------------
int
ps2netfs_req_dread(int fd, ps2netfs_dirent_t *dirent) { 
    pkt_dread_rly   *response;
    pkt_dread_req   *request;
    response        = (pkt_dread_rly *)&recv_packet[0];
    request         = (pkt_dread_req *)&send_packet[0];

    // Build the response packet.
    request->cmd = htonl(PS2NETFS_DREAD_CMD);
    request->len = htons(sizeof(pkt_dread_req));
    request->fd = htonl(fd);

    // Send the response packet.
    send(ps2_netfs_fd, request, sizeof(pkt_dread_req), 0);

    // now get the response
    if (!ps2netfs_accept_pkt(ps2_netfs_fd, (char *)response, 
            sizeof(pkt_dread_rly), PS2NETFS_DREAD_RLY)) {
        return -1;
    }
    // copy the values across
    memset(dirent, 0, sizeof(ps2netfs_dirent_t));
    dirent->mode   = ntohl(response->mode);
    dirent->attr   = ntohl(response->attr);
    dirent->size   = ntohl(response->size);
    dirent->hisize = ntohl(response->hisize);
    memcpy(dirent->ctime, response->ctime, 8*3);
    strncpy(dirent->name, response->path, MAX_PATH);
    return ntohl(response->retval);
}

// ------------------------------------------------------------------
int
ps2netfs_req_dclose(int fd) {
    pkt_file_rly    *response;
    pkt_close_req   *request;
    response        = (pkt_file_rly *)&recv_packet[0];
    request         = (pkt_close_req *)&send_packet[0];

    // Build the response packet.
    request->cmd = htonl(PS2NETFS_DCLOSE_CMD);
    request->len = htons(sizeof(pkt_close_req));
    request->fd = htonl(fd);

    // Send the response packet.
    send(ps2_netfs_fd, request, sizeof(pkt_close_req), 0);

    // now get the response
    if (!ps2netfs_accept_pkt(ps2_netfs_fd, (char *)response, 
            sizeof(pkt_file_rly), PS2NETFS_DCLOSE_RLY)) {
        return -1;
    }
    return ntohl(response->retval);
}

// ------------------------------------------------------------------
int
ps2netfs_req_devlist(char *buf) {
    pkt_devlist_rly *response;
    pkt_devlist_req *request;
    response        = (pkt_devlist_rly *)&recv_packet[0];
    request         = (pkt_devlist_req *)&send_packet[0];

    // Build the request packet.
    request->cmd = htonl(PS2NETFS_DEVLIST_CMD);
    request->len = htons(sizeof(pkt_devlist_req));
    request->path[0] = 0;
    request->path[1] = 0;

    // Send the request packet.
    send(ps2_netfs_fd, request, sizeof(pkt_devlist_req), 0);

    // now get the response
    if (!ps2netfs_accept_pkt(ps2_netfs_fd, (char *)response, 
            sizeof(pkt_devlist_rly), PS2NETFS_DEVLIST_RLY)) {
        return -1;
    }
    memcpy(buf, &response->list[0], MAX_PATH);
    return ntohl(response->retval);
}

// ------------------------------------------------------------------
int
ps2netfs_req_mount(char *fsname, char *devname, int flag, char *arg, int arglen) {
    pkt_file_rly    *response;
    pkt_mount_req   *request;
    response        = (pkt_file_rly *)&recv_packet[0];
    request         = (pkt_mount_req *)&send_packet[0];

    // Build the request packet.
    request->cmd = htonl(PS2NETFS_MOUNT_CMD);
    request->len = htons(sizeof(pkt_mount_req));
    strncpy(request->fsname, fsname, MAX_PATH);
    strncpy(request->devname, devname, MAX_PATH);
    request->flag = htonl(flag);
    strncpy(request->arg, arg, MAX_PATH);
    request->arglen = htonl(arglen);

    // Send the request packet.
    send(ps2_netfs_fd, request, sizeof(pkt_mount_req), 0);

    // now get the response
    if (!ps2netfs_accept_pkt(ps2_netfs_fd, (char *)response, 
            sizeof(pkt_mount_req), PS2NETFS_MOUNT_RLY)) {
        return -1;
    }
    return ntohl(response->retval);
}

// ------------------------------------------------------------------
int
ps2netfs_req_umount(char *path, int flags) {
    pkt_file_rly    *response;
    pkt_file_req    *request;
    response        = (pkt_file_rly *)&recv_packet[0];
    request         = (pkt_file_req *)&send_packet[0];

    // Build the request packet.
    request->cmd = htonl(PS2NETFS_UMOUNT_CMD);
    request->len = htons(sizeof(request));
    request->flags = flags;
    strncpy(request->path, path, MAX_PATH);

    // Send the request packet.
    send(ps2_netfs_fd, &request, sizeof(pkt_file_req), 0);

    // now get the response
    if (!ps2netfs_accept_pkt(ps2_netfs_fd, (char *)response, 
            sizeof(pkt_file_rly), PS2NETFS_UMOUNT_RLY)) {
        return -1;
    }
    return ntohl(response->retval);
}

// ------------------------------------------------------------------
int
ps2netfs_req_mkdir(char *path) {
    pkt_file_rly    *response;
    pkt_file_req    *request;
    response        = (pkt_file_rly *)&recv_packet[0];
    request         = (pkt_file_req *)&send_packet[0];

    // Build the request packet.
    request->cmd = htonl(PS2NETFS_MKDIR_CMD);
    request->len = htons(sizeof(pkt_file_req));
    strncpy(request->path, path, MAX_PATH);

    // Send the request packet.
    send(ps2_netfs_fd, request, sizeof(pkt_file_req), 0);

    // now get the response
    if (!ps2netfs_accept_pkt(ps2_netfs_fd, (char *)response, 
            sizeof(pkt_file_rly), PS2NETFS_MKDIR_RLY)) {
        return -1;
    }
    return ntohl(response->retval);
}

// ------------------------------------------------------------------
int
ps2netfs_req_rmdir(char *path) {
    pkt_file_rly    *response;
    pkt_file_req    *request;
    request         = (pkt_file_req *)&send_packet[0];
    response        = (pkt_file_rly *)&recv_packet[0];

    // Build the request packet.
    request->cmd = htonl(PS2NETFS_RMDIR_CMD);
    request->len = htons(sizeof(pkt_file_req));
    request->flags = htonl(0);
    strncpy(request->path, path, MAX_PATH);

    // Send the request packet.
    send(ps2_netfs_fd, request, sizeof(pkt_file_req), 0);

    // now get the response
    if (!ps2netfs_accept_pkt(ps2_netfs_fd, (char *)response, 
            sizeof(pkt_file_rly), PS2NETFS_RMDIR_RLY)) {
        return -1;
    }
    return ntohl(response->retval);
}

// ------------------------------------------------------------------
int
ps2netfs_req_sync(char *path,int flag) {
    pkt_file_req    *request;
    pkt_file_rly    *response;
    request         = (pkt_file_req *)&send_packet[0];
    response        = (pkt_file_rly *)&recv_packet[0];

    // Build the request packet.
    request->cmd = htonl(PS2NETFS_SYNC_CMD);
    request->len = htons(sizeof(pkt_file_req));
    request->flags = htonl(flag);
    strncpy(request->path, path, MAX_PATH);

    // Send the request packet.
    send(ps2_netfs_fd, request, sizeof(pkt_file_rly), 0);

    // now get the response
    if (!ps2netfs_accept_pkt(ps2_netfs_fd, (char *)response, 
            sizeof(pkt_file_rly), PS2NETFS_SYNC_RLY)) {
        return -1;
    }
    return ntohl(response->retval);
}

// ------------------------------------------------------------------
int
ps2netfs_req_delete(char *path, int flags) {
    pkt_file_rly    *response;
    pkt_file_req    *request;
    request         = (pkt_file_req *)&send_packet[0];
    response        = (pkt_file_rly *)&recv_packet[0];

    request->cmd = htonl(PS2NETFS_REMOVE_CMD);
    request->len = htonl(sizeof(pkt_file_req));
    request->flags = htonl(flags);
    if ( path ) {
        strncpy(request->path, path, MAX_PATH);
    }
    send(ps2_netfs_fd, request, sizeof(pkt_file_req), 0);

    if (!ps2netfs_accept_pkt(ps2_netfs_fd, (char *)response, 
            sizeof(pkt_file_rly), PS2NETFS_REMOVE_RLY)) {
        return -1;
    }
    return ntohl(response->retval);
}

// ------------------------------------------------------------------
int
ps2netfs_req_format(char *path, int flags) {
    pkt_file_rly    *response;
    pkt_file_req    *request;
    request         = (pkt_file_req *)&send_packet[0];
    response        = (pkt_file_rly *)&recv_packet[0];

    request->cmd    = htonl(PS2NETFS_REMOVE_CMD);
    request->len    = htonl(sizeof(pkt_file_req));
    request->flags  = htonl(flags);
    if ( path ) {
        strncpy(request->path, path, MAX_PATH);
    }
    send(ps2_netfs_fd, request, sizeof(pkt_file_req), 0);

    if (!ps2netfs_accept_pkt(ps2_netfs_fd, (char *)response, 
            sizeof(pkt_file_rly), PS2NETFS_FORMAT_RLY)) {
        return -1;
    }
    return ntohl(response->retval);
}

// ------------------------------------------------------------------
// Helper files for ps2net functions
void
ps2netfs_showMode(unsigned short mode) {
    int i;
    char *p, str[12];

    p = str;
    *p++ = (mode & FIO_S_IFDIR) ? 'd' : (mode & FIO_S_IFLNK) ? 'l': '-';
    for (i=8; i>=0; i--) {
        if ((1 << i) & mode) {
            if (i == 9) *p++ = 's';
            else if ((i%3) == 2) *p++ = 'r';
            else if ((i%3) == 1) *p++ = 'w';
            else if ((i%3) == 0) *p++ = 'x';
        } else *p++ = '-';
    }
    *p = '\0';
    printf("%s", str);
    return;
}

void
ps2netfs_showStat(ps2netfs_dirent_t *stat) {
    static char *strMonth[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",};
    unsigned long int size = ((unsigned long int)stat->hisize << 32) | stat->size;

    ps2netfs_showMode(stat->mode);
    printf(" 0x%04x %9d %s %2d %2d:%02d",
        stat->attr, (unsigned int)size,
        strMonth[stat->mtime[5]-1],
        stat->mtime[4], stat->mtime[3], stat->mtime[2]);
    return;
}
