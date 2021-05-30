#include "common.h"
#include "syscall.h"
#include "fs.h"
#include "memory.h"

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  //printf("call num=%d\n",a[0]);
  switch (a[0]) {
    case SYS_none:{
      SYSCALL_ARG1(r)=sys_none();
      break;
    }
    case SYS_exit:{
      sys_exit(SYSCALL_ARG4(r));
      break;
    }
    case SYS_write:{
      /*printf("hit write\n");
      printf("arg1=%08X\n",SYSCALL_ARG1(r));
      printf("arg2=%08X\n",SYSCALL_ARG2(r));
      printf("arg3=%08X\n",SYSCALL_ARG3(r));
      printf("arg4=%08X\n",SYSCALL_ARG4(r));*/
      SYSCALL_ARG1(r)=sys_write(SYSCALL_ARG4(r),(void*)SYSCALL_ARG2(r),SYSCALL_ARG3(r));
      break;
    }
    case SYS_open:{
      SYSCALL_ARG1(r)=sys_open((char*)SYSCALL_ARG4(r));
      break;
    }
    case SYS_brk:{
      SYSCALL_ARG1(r)=sys_sbrk(SYSCALL_ARG4(r));
      break;
    }
    case SYS_read:{
      SYSCALL_ARG1(r)=sys_read(SYSCALL_ARG4(r),(void*)SYSCALL_ARG2(r),SYSCALL_ARG3(r));
      break;
    }
    case SYS_close:{
      SYSCALL_ARG1(r)=sys_close(SYSCALL_ARG4(r));
      break;
    }
    case SYS_lseek:{
      SYSCALL_ARG1(r)=sys_lseek(SYSCALL_ARG4(r),SYSCALL_ARG2(r),SYSCALL_ARG3(r));
      break;
    }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}

int sys_none()
{
  return 1;
}

void sys_exit(int arg)
{
  _halt(arg);
}

int sys_sbrk(intptr_t increment)
{
  return mm_brk(increment);
}

int sys_write(int fd, void *buf, size_t count)
{
  if(fd==1||fd==2)
  {
    //Log("write ch");
    for(int i=0;i<count;i++)
    {
      _putc(((char*)buf)[i]);
    }
    return count;
  }
  else
  {
    return fs_write(fd,buf,count);
  }
  panic("panic at sys_write\n");
  return -1;
}

int sys_open(const char* file)
{
  return fs_open(file,0,0);
}
int sys_read(int fd , void*buf,size_t len)
{
  return fs_read(fd,buf,len);
}

int sys_close(int fd)
{
  return fs_close(fd);
}

int sys_lseek(int fd , off_t offset,int whence)
{
  return fs_lseek(fd,offset,whence);
}