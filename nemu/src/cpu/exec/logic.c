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
  //TODO();
  uint32_t result;
  if(decoding.dest.type==OP_TYPE_REG)
  {
    result = reg_l(decoding.dest.reg) ^= reg_l(decoding.src2.reg);
  }
  else if(decoding.dest.type==OP_TYPE_MEM)
  {
    uint32_t val = paddr_read(decoding.dest.addr,32);
    result = val ^= reg_l(decoding.src2.reg);
    paddr_write(decoding.dest.addr,32,val);
  }
  /*
    设置eflags
    CF = 0, OF = 0; SF, ZF, and PF as described in Appendix C; AF is undefined
    SF Sign Flag ── Set equal to high-order bit of result (0 is positive, 1 if negative).
    ZF Zero Flag ── Set if result is zero; cleared otherwise.
    PF Parity Flag ── Set if low-order eight bits of result contain an even number of 1 bits; cleared otherwise.
  */
  cpu.EFLAGS &= ~eflag_cf;
  cpu.EFLAGS &= ~eflag_of;
  if((result>>31))
  {
    cpu.EFLAGS |= eflag_sf;
  }
  else
  {
    cpu.EFLAGS &= ~eflag_sf;
  }
  if(result==0)
  {
    cpu.EFLAGS |= eflag_zf;
  }
  else
  {
    cpu.EFLAGS &= ~eflag_zf;
  }
  result = 0;
  uint8_t temp = cpu.EFLAGS;
  while(temp / 2 != 0)
  {
    result += temp % 2;
    temp = temp / 2;
  }
  if(temp % 2 == 0)
  {
    cpu.EFLAGS |= eflag_pf;
  }
  else 
  {
    cpu.EFLAGS &= ~eflag_pf;
  }
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
