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
ssize_t sys_write(int fd, void *buf, size_t count);
int sys_sbrk(intptr_t increment);
void sys_exit(int arg);
int sys_open(const char* file);
ssize_t sys_read(int fd , void*buf,size_t len);
int sys_close(int fd);
int sys_lseek(int fd , off_t offset,int whence);

#endif
