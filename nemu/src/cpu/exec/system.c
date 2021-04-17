#include "cpu/exec.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt) {
  //TODO();
  //printf("dest=%08X\n",id_dest->val);
  //printf("hit lidt\n");
  /*printf("width=%d\n",id_dest->width);
  printf("eip=%08X\n",cpu.eip);
  printf("imm=%08X,val=%08X\n",id_dest->imm,id_dest->val);
  printf("type=%d\n",id_dest->type);
  printf("reg=%d\n",id_dest->reg);
  printf("addr=%08X\n",id_dest->addr);*/
  /*if(id_dest->width==2)
  {
    uint32_t total=vaddr_read(id_dest->addr,4);
    cpu.idtr_limit = total>>24;
    cpu.idtr_base = (total<<16)>>16;
  }
  else
  {
    //printf("testaddr=%08X\n",vaddr_read(id_dest->addr,4));
    uint32_t low_four_byte=vaddr_read(id_dest->addr,4);//读出来的6byte数据中的低4byte
    uint16_t high_two_byte=vaddr_read(id_dest->addr+4,2);//读出来的6byte数据中的高2byte
    //printf("high=%08X,low=%08X\n",high_two_byte,low_four_byte);
    uint32_t temp_base=(low_four_byte<<8)>>8;
    //printf("tempbase=%08X\n",temp_base);
    uint16_t temp_limt=low_four_byte>>24;
    //printf("templimt=%08X\n",temp_limt);
    cpu.idtr_base = temp_base + (((uint32_t)(high_two_byte>>8))<<24);
    cpu.idtr_limit=temp_limt+(high_two_byte<<8);
    //printf("limt=%08X\n",limt);
    cpu.idtr_limit=vaddr_read(id_dest->addr,2);
    cpu.idtr_base=vaddr_read(id_dest->addr+2,4);
    printf("limt=%08X\n",cpu.idtr_limit);
    printf("base=%08X\n",cpu.idtr_base);
  }*/
  rtl_li(&t0,id_dest->addr);
  rtl_li(&cpu.idtr_limit, vaddr_read(t0,2));
  rtl_li(&cpu.idtr_base, vaddr_read(t0+2,4));
  printf("hit lidt\n");
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
  TODO();
  printf("hit int\n");
  raise_intr(id_dest->imm,cpu.eip);
  print_asm("int %s", id_dest->str);
  

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  TODO();

  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
  //TODO();
  t0=pio_read(id_src->val,1);
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
  pio_write(id_dest->val,1,id_src->val);
  print_asm_template2(out);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}
