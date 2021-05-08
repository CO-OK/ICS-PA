#ifndef __SYSCALL_H__
#define __SYSCALL_H__

enum {
  SYS_none,
  SYS_open,
  SYS_read,
  SYS_write,
  SYS_exit,
  SYS_kill,
  SYS_getpid,
  SYS_close,
  SYS_lseek,
  SYS_brk,
  SYS_fstat,
  SYS_time,
  SYS_signal,
  SYS_execve,
  SYS_fork,
  SYS_link,
  SYS_unlink,
  SYS_wait,
  SYS_times,
  SYS_gettimeofday
};
int sys_none();
int sys_exit(int arg);
ssize_t sys_write(int fd, void *buf, size_t count);
ssize_t fs_write(int fd, void *buf, size_t count);
int sys_sbrk(intptr_t increment);
int fs_open(char*path);
ssize_t fs_read(int fd, void *buf, size_t count);
int fs_close(int fd);
ssize_t fs_filesz(int fd);
off_t lseek(int fd, off_t offset, int whence);
#endif
