#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  //TODO();
  /*
    依次将 EFLAGS,CS,EIP 寄存器的值压入堆栈
    从 IDTR 中读出 IDT 的首地址
    根据异常(中断)号在 IDT 中进行索引,找到一个门描述符
    将门描述符中的 offset 域组合成目标地址
    跳转到目标地址
  */
  //printf("hit raise_intr\n");
  rtl_push(&cpu.EFLAGS_,4);
  rtl_push(&cpu.cs,2);
  rtl_push(&cpu.eip,4);
  vaddr_t idt_base_addr=cpu.idtr_base;
  vaddr_t gate_enrty= cpu.idtr_base + sizeof(GateDesc)*NO;
  GateDesc *point=gate_enrty;
  //uint32_t off15to0 = vaddr_read(gate_enrty,4);
  //uint32_t off16to32 = vaddr_read(gate_enrty+64,4);
  uint16_t off15to0 = point->offset_15_0;
  uint16_t off16to32 = point->offset_31_16;
  rtlreg_t temp_eip=point->offset_31_16;
  temp_eip=temp_eip>>16;
  temp_eip+=point->offset_15_0;
  cpu.eip=temp_eip;
}

void dev_raise_intr() {
  printf("hit dev_raise_intr\n");
}
