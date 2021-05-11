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
  //Log("hit raise_intr with NO=%X",NO);
  rtl_push(&cpu.EFLAGS_,4);
  rtl_push(&cpu.cs,4);
  rtl_push(&ret_addr,4);
  //printf("eip==%08X\n",cpu.eip);
  vaddr_t gate_enrty= cpu.idtr_base + sizeof(GateDesc)*NO;
  //printf("gate entry=%08X\n",gate_enrty);
  uint32_t off15to0 = vaddr_read(gate_enrty,2);
  uint32_t off16to32 = vaddr_read(gate_enrty+6,8);
  //printf("15t0=%08X 16t32=%08X\n",off15to0,off16to32);
  //printf("temp=%08X\n",off16to32>>16);
  vaddr_t final=(off16to32<<16)+(0x0000ffff&off15to0);
  //printf("final=%08X\n",final);
  //decoding.seq_eip=final;
  decoding.is_jmp=1;
  decoding.jmp_eip=final;
  //cpu.eip=final;
  //Log("in raise_intr eax=%08X,ebx=%08X,ecx=%08X,edx=%08X",cpu.eax,cpu.ebx,cpu.ecx,cpu.edx);
  //Log("in raise_intr esp=%08X ebp=%08X",cpu.esp,cpu.ebp);
  //Log("finish raise_intr with NO=%X target addr = %08X",NO,final);

}

void dev_raise_intr() {
}
