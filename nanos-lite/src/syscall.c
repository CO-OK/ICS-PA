#include "common.h"
#include "syscall.h"
extern char _end;

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
    }
    case SYS_write:{
      /*printf("hit write\n");
      printf("arg1=%08X\n",SYSCALL_ARG1(r));
      printf("arg2=%08X\n",SYSCALL_ARG2(r));
      printf("arg3=%08X\n",SYSCALL_ARG3(r));
      printf("arg4=%08X\n",SYSCALL_ARG4(r));*/
      //return 0;
      printf("hit write\n");
      sys_write(SYSCALL_ARG4(r),SYSCALL_ARG2(r),SYSCALL_ARG3(r));
      break;
    }
    case SYS_brk:{
      //printf("hit brk\n");
      printf("arg4=%08X\n",SYSCALL_ARG4(r));
      return sys_sbrk(SYSCALL_ARG4(r));
      printf("end\n");
      return 0;
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
  //printf("fd=%d\ncount=%d\n",fd,count);
  if(fd==1||fd==2)
  {
    for(int i=0;i<count;i++)
    {
      _putc(buf[i]);
    }
    return count;
  }
  return -1;
}

int sys_sbrk(intptr_t increment)
{
  intptr_t program_break = (intptr_t)&_end;
  intptr_t old_pb = program_break;
  program_break += increment;	
  printf("break is %08X\n",program_break);
	return (void *)old_pb;
  //return 0;
}