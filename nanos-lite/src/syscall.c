#include "common.h"
#include "syscall.h"

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  //printf("call num=%d\n",a[0]);
  switch (a[0]) {
    case 0:{
      SYSCALL_ARG1(r)=sys_none();
      break;
    }
    case 4:{
      sys_exit(SYSCALL_ARG3(r));
      break;
     // printf("hit 4");
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