#include "common.h"
#include "syscall.h"

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
      return sys_exit(SYSCALL_ARG3(r));
      break;
     // printf("hit 4");
    }
    case SYS_write:{
      printf("hit write\n");
      printf("arg1=%08X\n",SYSCALL_ARG1(r));
      printf("arg2=%08X\n",SYSCALL_ARG2(r));
      printf("arg3=%08X\n",SYSCALL_ARG3(r));
      printf("arg4=%08X\n",SYSCALL_ARG4(r));
      return sys_write(SYSCALL_ARG2(r),SYSCALL_ARG3(r),SYSCALL_ARG4(r));
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

int sys_exit(int arg)
{
  _halt(arg);
}

int sys_write(int fd, char *buf, size_t count)
{
  printf("fd=%d\ncount=%d\n",fd,count);
  if(fd==1||fd==2)
  {
    for(int i=0;i<count;i++)
    {
      _putc(buf[i]);
    }
  }
  return count;
}