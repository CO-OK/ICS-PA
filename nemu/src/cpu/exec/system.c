#include "cpu/exec.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt) {
  //TODO();
  if(id_dest->width==2)
  {
    uint32_t total=vaddr_read(id_dest->addr,4);
    cpu.idtr_limit = total>>24;
    cpu.idtr_base = (total<<16)>>16;
  }
  else
  {
    cpu.idtr_limit=vaddr_read(id_dest->addr,4);
    cpu.idtr_base=vaddr_read(id_dest->addr+2,4);
    /*uint32_t low4=vaddr_read(id_dest->addr,4);
    uint32_t high4=vaddr_read(id_dest->addr+2,4);
    printf("high=%08X,low=%08X\n",high4&0xff000000,low4&0x00ffffff);
    cpu.idtr_limit = (high4>>8)&0x0000ffff;
    cpu.idtr_base=(low4&0x00ffffff)+(high4&0xff000000);*/
    //printf("limt=%08X\n",cpu.idtr_limit);
    //printf("base=%08X\n",cpu.idtr_base);

  }
  //printf("hit lidt\n");
  //printf("destaddr=%08X\n",id_dest->addr);
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(int) {
  //TODO();
  //printf("hit int\n");
  //printf("int num=%08X,eip=%08X\n",id_dest->imm,cpu.eip);
  //printf("eip=%08X\nseq_eip=%08X\n",cpu.eip,decoding.seq_eip);
  raise_intr(id_dest->imm,decoding.seq_eip);
  print_asm("int %s", id_dest->str);
  

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  //TODO();
  rtl_pop(&decoding.seq_eip);
  rtl_pop(&cpu.cs);
  rtl_pop(&cpu.EFLAGS_);
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
