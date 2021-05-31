#include "cpu/exec.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt) {
  //TODO();
  
  cpu.idtr_limit=vaddr_read(id_dest->addr,2);
  cpu.idtr_base=vaddr_read(id_dest->addr+2,4);
  Log("load lidt limit=%08X,base=%08X",cpu.idtr_limit,cpu.idtr_base);
  //printf("hit lidt\n");
  //printf("destaddr=%08X\n",id_dest->addr);
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  //TODO();
  switch (id_dest->reg) {
		case 0: {cpu.CR0 = id_src->val; break;}
		case 3: {cpu.CR3 = id_src->val; break;}
		default: panic("mov_r2cr");
	}
  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}
make_EHelper(mov_cr2r) {
  //TODO();
  switch (id_src->reg) {
		case 0: {t0 = cpu.CR0; break;}
		case 3: {t0 = cpu.CR3; break;}
		default: panic("mov_cr2r");
	}
	operand_write(id_dest, &t0);
  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(int) {
  //Log("touch int %X",id_dest->imm);
  raise_intr(id_dest->val,decoding.seq_eip);
  //Log("int %X finished , retAddr=%08X",id_dest->imm,decoding.seq_eip);
  print_asm("int %s", id_dest->str);
}

make_EHelper(iret) {
  rtl_pop(&decoding.jmp_eip);
	decoding.is_jmp=1;
	rtl_pop(&t0);
	cpu.cs = t0 & 0xffff;
	rtl_pop(&cpu.EFLAGS_);
  //Log("finish iret eip=%08X",cpu.eip);
  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
  //TODO();
  t0=pio_read(id_src->val,id_dest->width);
  operand_write(id_dest,&t0);
  print_asm_template2(in);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(out) {
  //TODO();
  //printf("hit\n");
  //printf("width=%d\n",id_dest->width);
  pio_write(id_dest->val,id_src->width,id_src->val);
  print_asm_template2(out);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}
