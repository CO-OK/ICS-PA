#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push_reg) {
  //printf("val=%08X\n",id_dest->val);
  /*
    push imm8 指令需要对立即数进行符号扩展
  */
  //printf("opcode=%08X\n",decoding.opcode);
  if(decoding.opcode==0x68||decoding.opcode==0x6a)
  {
    rtl_sext(&t0,&id_dest->val,id_dest->width);
    id_dest->val=t0;
  }
  //if(decoding.opcode)
  rtl_push(&id_dest->val,id_dest->width);
  print_asm_template1(push);
}


make_EHelper(pop_reg) {
  rtl_pop(&t0);
  operand_write(id_dest,&t0);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  //TODO();
  rtlreg_t temp=cpu.esp;
  rtl_push(&cpu.eax,4);
  rtl_push(&cpu.ecx,4);
  rtl_push(&cpu.edx,4);
  rtl_push(&cpu.ebx,4);
  rtl_push(&temp,4);
  rtl_push(&cpu.ebp,4);
  rtl_push(&cpu.esi,4);
  rtl_push(&cpu.edi,4);
  //printf("pusha %08X\n",cpu.esp);
  print_asm("pusha");
}

make_EHelper(popa) {
  //TODO();
  //printf("before popa %08X\n",cpu.esp);
  rtlreg_t temp;
  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&temp);
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);
  //printf("popa %08X\n",cpu.esp);
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
  //printf("src_width=%d\ndest_width=%d\nval=%08X\n",id_src->width,id_dest->width,id_src->val);
  //printf("t21=%08X\n",t2);
  uint32_t t = decoding.opcode & 0x000000ff;
  if(t==0xbe)
  {
    //Move byte to dword, sign-extend
    rtl_sext(&t2, &id_src->val, 1);
  }
  else if(t==0xbf)
  {
    //Move word to dword, sign-extend
    rtl_sext(&t2, &id_src->val, 2);
  }
  
  //printf("t22=%08X\n",t2);
  //id_dest->width=id_src->width;
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  //printf("op=%X\n",decoding.opcode);
  /*
    这里应该根据操作码来决定操作数的宽度？
  */
  uint32_t t = decoding.opcode & 0x000000ff;
  //printf("op=%X\n",t);
  if(t==0xb6)
  {
    //Move byte to dword, zero-extend
    id_src->val &= 0x000000ff;
  }
  else if(t==0xb7)
  {
    //Move word to dword, zero-extend
    id_src->val &= 0x0000ffff;
  }

  //printf("dest_width=%d\n",id_dest->width);
  //printf("src_width=%d\n",id_src->width);
  //printf("val=%08X\n",id_src->val);
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  
  rtl_li(&t2, id_src->addr);
  //printf("width=%d\n",id_dest->width);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
