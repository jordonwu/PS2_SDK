// (C) 2004 by Khaled Daham, <khaled@w-arts.com>

int batch_io_loop(fd_set *master_set, int maxfd, int nservice, int timeout);
int batch_io_read(int fd);
int batch_log_read(int fd);
