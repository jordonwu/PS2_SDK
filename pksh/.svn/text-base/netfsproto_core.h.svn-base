/* -*- C -*-
 * (C) 2004 by Adresd, <adresd_ps2dev@yahoo.com>
 * (C) 2004 by Khaled Daham
 */

#ifndef _NETFS
#define _NETFS

#pragma pack(1)
#define PACKET_MAXSIZE  4096

#define PS2NETFS_LISTEN_PORT  0x4713

// from iomanx, all operations
//  open
#define PS2NETFS_OPEN_CMD     0xbeef8011
#define PS2NETFS_OPEN_RLY     0xbeef8012
//  close
#define PS2NETFS_CLOSE_CMD    0xbeef8021
#define PS2NETFS_CLOSE_RLY    0xbeef8022
//  read
#define PS2NETFS_READ_CMD     0xbeef8031
#define PS2NETFS_READ_RLY     0xbeef8032
//  write
#define PS2NETFS_WRITE_CMD    0xbeef8041
#define PS2NETFS_WRITE_RLY    0xbeef8042
//  lseek
#define PS2NETFS_LSEEK_CMD    0xbeef8051
#define PS2NETFS_LSEEK_RLY    0xbeef8052
//  ioctl
#define PS2NETFS_IOCTL_CMD    0xbeef8061
#define PS2NETFS_IOCTL_RLY    0xbeef8062
//  remove
#define PS2NETFS_REMOVE_CMD   0xbeef8071
#define PS2NETFS_REMOVE_RLY   0xbeef8072
//  mkdir
#define PS2NETFS_MKDIR_CMD    0xbeef8081
#define PS2NETFS_MKDIR_RLY    0xbeef8082
//  rmdir
#define PS2NETFS_RMDIR_CMD    0xbeef8091
#define PS2NETFS_RMDIR_RLY    0xbeef8092
//  dopen
#define PS2NETFS_DOPEN_CMD    0xbeef80A1
#define PS2NETFS_DOPEN_RLY    0xbeef80A2
//  dclose
#define PS2NETFS_DCLOSE_CMD   0xbeef80B1
#define PS2NETFS_DCLOSE_RLY   0xbeef80B2
//  dread
#define PS2NETFS_DREAD_CMD    0xbeef80C1
#define PS2NETFS_DREAD_RLY    0xbeef80C2
//  getstat
#define PS2NETFS_GETSTAT_CMD  0xbeef80D1
#define PS2NETFS_GETSTAT_RLY  0xbeef80D2
//  chstat
#define PS2NETFS_CHSTAT_CMD   0xbeef80E1
#define PS2NETFS_CHSTAT_RLY   0xbeef80E2
//  format
#define PS2NETFS_FORMAT_CMD   0xbeef80F1
#define PS2NETFS_FORMAT_RLY   0xbeef80F2
// extended commands
//  rename
#define PS2NETFS_RENAME_CMD   0xbeef8111
#define PS2NETFS_RENAME_RLY   0xbeef8112
//  chdir
#define PS2NETFS_CHDIR_CMD    0xbeef8121
#define PS2NETFS_CHDIR_RLY    0xbeef8122
//  sync
#define PS2NETFS_SYNC_CMD     0xbeef8131
#define PS2NETFS_SYNC_RLY     0xbeef8132
//  mount
#define PS2NETFS_MOUNT_CMD    0xbeef8141
#define PS2NETFS_MOUNT_RLY    0xbeef8142
//  umount
#define PS2NETFS_UMOUNT_CMD   0xbeef8151
#define PS2NETFS_UMOUNT_RLY   0xbeef8152
//  lseek64
#define PS2NETFS_LSEEK64_CMD  0xbeef8161
#define PS2NETFS_LSEEK64_RLY  0xbeef8162
//  devctl
#define PS2NETFS_DEVCTL_CMD   0xbeef8171
#define PS2NETFS_DEVCTL_RLY   0xbeef8172
//  symlink
#define PS2NETFS_SYMLINK_CMD  0xbeef8181
#define PS2NETFS_SYMLINK_RLY  0xbeef8182
//  readlink
#define PS2NETFS_READLINK_CMD 0xbeef8191
#define PS2NETFS_READLINK_RLY 0xbeef8192
//  ioctl2
#define PS2NETFS_IOCTL2_CMD   0xbeef81A1
#define PS2NETFS_IOCTL2_RLY   0xbeef81A2
// added on
//  info/status
#define PS2NETFS_INFO_CMD     0xbeef8F01
#define PS2NETFS_INFO_RLY     0xbeef8F02
//  fstype
#define PS2NETFS_FSTYPE_CMD   0xbeef8F11
#define PS2NETFS_FSTYPE_RLY   0xbeef8F12
//  devlist
#define PS2NETFS_DEVLIST_CMD  0xbeef8F21
#define PS2NETFS_DEVLIST_RLY  0xbeef8F22

// File open flags
#define PS2NETFS_O_RDONLY       0x0001
#define PS2NETFS_O_WRONLY       0x0002
#define PS2NETFS_O_RDWR         0x0003
#define PS2NETFS_O_DIROPEN      0x0008  // Internal use for dopen
#define PS2NETFS_O_NBLOCK       0x0010
#define PS2NETFS_O_APPEND       0x0100
#define PS2NETFS_O_CREAT                0x0200
#define PS2NETFS_O_TRUNC                0x0400

// Access flags for filesystem mount
#define PS2NETFS_FIO_MT_RDWR                    0x00
#define PS2NETFS_FIO_MT_RDONLY          0x01

#define PS2NETFS_SEEK_SET       0
#define PS2NETFS_SEEK_CUR       1
#define PS2NETFS_SEEK_END       2

// These are ps2 mode flags, for dirent mode
// File mode flags
#define FIO_S_IFMT              0xF000          // Format mask
#define FIO_S_IFLNK             0x4000          // Symbolic link
#define FIO_S_IFREG             0x2000          // Regular file
#define FIO_S_IFDIR             0x1000          // Directory

// Access rights
#define FIO_S_ISUID             0x0800          // SUID
#define FIO_S_ISGID             0x0400          // SGID
#define FIO_S_ISVTX             0x0200          // Sticky bit

#define FIO_S_IRWXU             0x01C0          // User access rights mask
#define FIO_S_IRUSR             0x0100          // read
#define FIO_S_IWUSR             0x0080          // write
#define FIO_S_IXUSR             0x0040          // execute

#define FIO_S_IRWXG             0x0038          // Group access rights mask
#define FIO_S_IRGRP             0x0020          // read
#define FIO_S_IWGRP             0x0010          // write
#define FIO_S_IXGRP             0x0008          // execute

#define FIO_S_IRWXO             0x0007          // Others access rights mask
#define FIO_S_IROTH             0x0004          // read
#define FIO_S_IWOTH             0x0002          // write
#define FIO_S_IXOTH             0x0001          // execute

#define FIO_LSEEK_SET   0x0000
#define FIO_LSEEK_CURRENT 0x0001
#define FIO_LSEEK_END   0x0002

int ps2_netfs_fd;
char send_packet[PACKET_MAXSIZE] __attribute__((aligned(16)));
char recv_packet[PACKET_MAXSIZE] __attribute__((aligned(16)));

typedef struct { 
	unsigned int	mode;
	unsigned int	attr;
	unsigned int	size; 
	unsigned char	ctime[8];
	unsigned char	atime[8];
	unsigned char	mtime[8]; 
	unsigned int	hisize; 
	unsigned char	name[256]; 
} ps2netfs_dirent_t;

typedef struct {
    unsigned int cmd;
    unsigned short len;
    unsigned int retval;
} ps2netfs_file_rly;

/* Command Functions */
int ps2netfs_commandhandler(char *host,char *command,char *filename, char *filename2);
int ps2netfs_cmd_dir(char *host,char *path);
int ps2netfs_cmd_devlist(char *host);
int ps2netfs_cmd_copytops2(char *host,char *pathpc, char *pathps2);
int ps2netfs_cmd_copyfromps2(char *host,char *pathps2, char *pathpc);
int ps2netfs_cmd_mount(char *host,char *fsname, char *devname);
int ps2netfs_cmd_unmount(char *host,char *path); 
int ps2netfs_cmd_mkdir(char *host,char *path);
int ps2netfs_cmd_sync(char *host,char *device);

/* Helper Functions */

void ps2netfs_showStat(ps2netfs_dirent_t *stat);
void ps2netfs_showMode(unsigned short mode);

/* Request Functions */
int ps2netfs_req_open(char *path, int mode);
int ps2netfs_req_close(int fd);
int ps2netfs_req_read(int fd, void *ptr, int size);
int ps2netfs_req_write(int fd, void *ptr, int size);
int ps2netfs_req_lseek(int fd, int offset, int whence);
int ps2netfs_req_dopen(char *path);
int ps2netfs_req_dclose(int fd);
int ps2netfs_req_dread(int fd, ps2netfs_dirent_t *buf);
int ps2netfs_req_devlist(char *buf);

int ps2netfs_req_mount(char *fsname, char *devname, int flag, char *arg, int arglen);
int ps2netfs_req_umount(char *path, int flags);

int ps2netfs_req_format(char *path, int flags);
int ps2netfs_req_delete(char *path, int flags);
int ps2netfs_req_mkdir(char *path);
int ps2netfs_req_rmdir(char *path);
int ps2netfs_req_sync(char *path,int flag);
#endif
