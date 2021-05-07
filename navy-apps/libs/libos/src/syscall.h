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
int sys_write(int fd, char *buf, size_t count);
int sys_sbrk(intptr_t increment);
int fs_open(char*path);
int fs_read(int fd, void *buf, size_t count);
int fs_close(int fd);
ssize_t fs_filesz(int fd);
#endif
