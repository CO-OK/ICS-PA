#include "common.h"
#include "syscall.h"

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  for(int i=0;i<4;i++)
    printf("a%d=%08X\n",i,a[i]);
  //printf("call num=%d\n",a[0]);
  switch (a[0]) {
    case 0:{
      a[7]=sys_none();
      break;
    }
    case 4:{
      printf("hit 4");
    }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}

int sys_none()
{
  return 5;
}