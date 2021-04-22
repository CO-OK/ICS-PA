#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  TODO();
  /*
    依次将 EFLAGS,CS,EIP 寄存器的值压入堆栈
    从 IDTR 中读出 IDT 的首地址
    根据异常(中断)号在 IDT 中进行索引,找到一个门描述符
    将门描述符中的 offset 域组合成目标地址
    跳转到目标地址
  */
}

void dev_raise_intr() {
  //printf("hit dev_raise_intr\n");
}
