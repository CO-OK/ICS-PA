#ifndef __ARCH_H__
#define __ARCH_H__

#include <am.h>

#define PMEM_SIZE (128 * 1024 * 1024)
#define PGSIZE    4096    // Bytes mapped by a page

struct _RegSet {
  //uintptr_t esi, ebx, eax, eip, edx, error_code, eflags, ecx, cs, esp, edi, ebp;
  uintptr_t eflags;
  uintptr_t cs;
  uintptr_t eip;
  uintptr_t error_code;
  int       irq;
  uintptr_t eax;
  uintptr_t ecx;
  uintptr_t edx;
  uintptr_t ebx;
  uintptr_t temp;
  uintptr_t ebp;
  uintptr_t esi;
  uintptr_t edi;
  uintptr_t esp;
  
  
};

#define SYSCALL_ARG1(r) 0
#define SYSCALL_ARG2(r) 0
#define SYSCALL_ARG3(r) 0
#define SYSCALL_ARG4(r) 0

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#endif
