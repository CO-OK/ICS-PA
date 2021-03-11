#include "cpu/exec.h"

make_EHelper(test) {
  TODO();

  print_asm_template2(test);
}

make_EHelper(and) {
  TODO();

  print_asm_template2(and);
}

make_EHelper(xor) {
  TODO();

  print_asm_template2(xor);
}

make_EHelper(or) {
  //TODO();
  //80386 手册54
  if(decoding.src.type==OP_TYPE_REG)
  {
    switch (decoding.src2.type)
    {
      case OP_TYPE_MEM:{
        uint32_t val = paddr_read(decoding.src2.addr,32);
        reg_l(decoding.src.reg) |= val;
        break;
      }
      case OP_TYPE_REG:{
        reg_l(decoding.src.reg) |= reg_l(decoding.src2.reg);
        break;
      }
      case OP_TYPE_IMM:{
        reg_l(decoding.src.reg) |= decoding.src2.imm;
        break;
      }
    }
  }
  else if(decoding.src.type==OP_TYPE_MEM)
  {
    uint32_t val = paddr_read(decoding.src.addr,32);
    switch(decoding.src2.type)
    {
      case OP_TYPE_REG:{
        val|= reg_l(decoding.src2.reg);
        break;
      }
      case OP_TYPE_IMM:{
        uint32_t val2 = paddr_read(decoding.src2.addr,32);
        val |= val2;
        break;
      }
    }
    paddr_write(decoding.src.addr,32,val);
  }
  
  print_asm_template2(or);
}

make_EHelper(sar) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  TODO();

  print_asm_template1(not);
}
