#include "cpu/exec.h"

void cal_and()
{

}
make_EHelper(test) {
  //TODO();
  /*
    TEST computes the bit-wise logical AND of its two operands. Each bit
    of the result is 1 if both of the corresponding bits of the operands are 1;
    otherwise, each bit is 0. The result of the operation is discarded and only
    the flags are modified.
  */
  //opcode 是 85时只有32位和16位两种情况
  get_mr_value(&t1,id_dest);
  get_mr_value(&t2,id_src);
  if(id_dest->width==2)
  {
    t0 = (short)t1 & (short)t2;
  }
  else
  {
    t0 = t1 & t2;
  }
  /*
    OF = 0, CF = 0; SF, ZF, and PF as described in Appendix C
  */
  rtl_unset_CF(&eflag_CF);
  rtl_unset_OF(&eflag_OF);
  rtl_update_ZFSF(&t0,id_dest->width);
  print_asm_template2(test);
}

make_EHelper(and) {
  //TODO();
  /*
    注:应该是与源操作数的宽度相同,在执行  and    $0xfffffff0,%esp（这个指令应该是不合理的）时
    解码后目的操作数是4位，原操作数是2位，应该需要扩展
  */
  if(id_dest->type==OP_TYPE_REG)
  {
    if(id_src->width==1)
    {
      id_src->val=(char)id_src->val;
      
    }
    else if(id_src->width==2)
    {
      id_src->val=(short)id_src->val;
    }
    rtl_and(&t0,&id_dest->val,&id_src->val);
    operand_write(id_dest,&t0);
  }
  /*
    update eflags
    AND, OR, and XOR clear OF and CF, leave AF undefined, and update SF, ZF,
    and PF.
  */
  rtl_unset_OF(&eflag_OF);
  rtl_unset_CF(&eflag_CF);
  rtl_update_ZFSF(&t0,id_dest->width);

  print_asm_template2(and);
}

make_EHelper(xor_31) {
  //TODO();
  if(id_dest->type==OP_TYPE_REG)
  {
    rtl_xor(&t0,&id_dest->val,&id_src->val);
    operand_write(id_dest,&t0);
  }
  else if(id_dest->type==OP_TYPE_MEM)
  {
    t1 = paddr_read(id_dest->addr,id_dest->width);
    rtl_xor(&t0,&t1,&id_src->val);
    operand_write(id_dest,&t0);
  }
  /*
    设置eflags
    CF = 0, OF = 0; SF, ZF, and PF as described in Appendix C; AF is undefined
    SF Sign Flag ── Set equal to high-order bit of result (0 is positive, 1 if negative).
    ZF Zero Flag ── Set if result is zero; cleared otherwise.
    PF Parity Flag ── Set if low-order eight bits of result contain an even number of 1 bits; cleared otherwise.
  */
  //ZF SF
  rtl_update_ZFSF(&t0,id_dest->width);
  //PF 不管
  //CF
  rtl_unset_CF(&eflag_CF);
  //OF
  rtl_unset_OF(&eflag_OF);
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
  //TODO();
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
