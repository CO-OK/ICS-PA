#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push_reg) {
  rtl_push(&id_dest->val,4);
  print_asm_template1(push);
}


make_EHelper(pop_reg) {
  rtl_pop(&t0);
  operand_write(id_dest,&t0);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  //TODO();
  /*
    IF StackAddrSize = 16
    THEN
    SP ← BP;
    ELSE (* StackAddrSize = 32 *)
    ESP ← EBP;
    FI;
    IF OperandSize = 16
    THEN
    BP ← Pop();
    ELSE (* OperandSize = 32 *)
    EBP ← Pop();
    FI;
  */

  cpu.esp=cpu.ebp;
  rtl_pop(&cpu.ebp);
  print_asm("leave");
}

make_EHelper(cltd) {
  //printf("width=%d\n",id_dest->width);
  if (decoding.is_operand_size_16) {
    //TODO();
    rtl_msb(&t0,&cpu.eax,2);
    if(t0==1)//ax<0
    {
      cpu.edx |= 0x0000ffff;
    }
    else
    {
      cpu.edx &= 0x11110000;
    }
  }
  else {
    rtl_msb(&t0,&cpu.eax,4);
    if(t0==1)//ax<0
    {
      cpu.edx |= 0xffffffff;
    }
    else
    {
      cpu.edx = 0;
    }
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  printf("src_width=%d\ndest_width=%d\nval=%08X\n",id_src->width,id_dest->width,id_src->val);
  printf("t21=%08X\n",t2);
  rtl_sext(&t2, &id_src->val, id_src->width);
  if(id_src->width==4)
  {
    id_dest->width=2;
  }
  printf("t22=%08X\n",t2);
  //id_dest->width=id_src->width;
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  //printf("width=%d\n",id_dest->width);
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  
  rtl_li(&t2, id_src->addr);
  //printf("width=%d\n",id_dest->width);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
