#ifndef __FS_H__
#define __FS_H__

#include "common.h"

enum {SEEK_SET, SEEK_CUR, SEEK_END};

ssize_t fs_write(int fd, void *buf, size_t count);
int fs_open(char*path,int flags,int mode);
ssize_t fs_read(int fd, void *buf, size_t count);
int fs_close(int fd);
ssize_t fs_filesz(int fd);
off_t fs_lseek(int fd, off_t offset, int whence);
off_t get_open_offset(int fd);
void set_open_offset(int fd , off_t n);
#endif
