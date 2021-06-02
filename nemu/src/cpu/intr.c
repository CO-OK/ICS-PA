#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  //Log("hit raise_intr with NO=%X",NO);
  rtl_push(&cpu.EFLAGS_,4);
  e_IF=false;//拒绝响应中断
  rtl_push(&cpu.cs,4);
  rtl_push(&ret_addr,4);
  vaddr_t gate_enrty= cpu.idtr_base + sizeof(GateDesc)*NO;
  uint32_t off15to0 = vaddr_read(gate_enrty,2);
  uint32_t off16to32 = vaddr_read(gate_enrty+6,8);
  vaddr_t final=(off16to32<<16)+(0x0000ffff&off15to0);
  decoding.seq_eip=final;
  decoding.is_jmp=1;
  decoding.jmp_eip=final;
  cpu.eip=final;
  //Log("in raise_intr eax=%08X,ebx=%08X,ecx=%08X,edx=%08X",cpu.eax,cpu.ebx,cpu.ecx,cpu.edx);
  //Log("in raise_intr esp=%08X ebp=%08X",cpu.esp,cpu.ebp);
  //Log("finish raise_intr with NO=%X target addr = %08X eip=%08X",NO,final,cpu.eip);

}

void dev_raise_intr() {
  cpu.INTR=true;
}
