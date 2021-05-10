#include "cpu/exec.h"

int my_power(int base,int e)
{
  int res = 1;
  for(int i=0;i<e;i++)
    res *= base;
  return res;
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
  /*get_mr_value(&t1,id_dest);
  get_mr_value(&t2,id_src);
  if(id_dest->width==2)
  {
    t0 = (short)t1 & (short)t2;
  }
  else if(id_dest->width==4)
  {
    t0 = t1 & t2;
  }
  else
  {
    t0 = (unsigned char)t1 & (unsigned)t2;
  }*/
  /*
    OF = 0, CF = 0; SF, ZF, and PF as described in Appendix C
  */
  rtl_and(&t0,&id_dest->val,&id_src->val);
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
  //printf("destval=%08X\nsrcval=%08X\n",id_dest->val,id_src->val);
  rtl_and(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
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
  //printf("hit\n");
  rtl_xor(&t1, &id_dest->val, &id_src->val);
	rtl_unset_OF(&eflag_OF);
	rtl_unset_CF(&eflag_CF);
	rtl_update_ZFSF(&t1, id_dest->width);
	operand_write(id_dest, &t1);
  print_asm_template2(xor);
}

make_EHelper(or) {
  //TODO();
  //80386 手册54
  rtl_or(&t2,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t2);
  rtl_update_ZFSF(&t2,id_dest->width);
  rtl_unset_OF(&eflag_OF);
  rtl_unset_CF(&eflag_CF);
  print_asm_template2(or);
}

make_EHelper(sar) {
  //TODO();
  /*
    SAR and SHR shift the bits of the operand downward. The low-order
    bit is shifted into the carry flag. The effect is to divide the operand by
    2. SAR performs a signed divide with rounding toward negative infinity (not
    the same as IDIV); the high-order bit remains the same. SHR performs an
    unsigned divide; the high-order bit is set to 0.
  */
  rtl_sext(&t2,&id_dest->val,id_dest->width);
  rtl_sar(&t2,&t2,&id_src->val);
  operand_write(id_dest,&t2);
  rtl_update_ZFSF(&t2,id_dest->width);
  print_asm_template2(sar);
}

make_EHelper(shl) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  /*
    SAL (or its synonym, SHL) shifts the bits of the operand upward. The
    high-order bit is shifted into the carry flag, and the low-order bit is set
    to 0.
  */

  rtl_shl(&t2,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t2);
  rtl_update_ZFSF(&t2,id_dest->width);
  print_asm_template2(shl);
}

make_EHelper(shr) {
  //TODO();
  rtl_shr(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0,id_dest->width);
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  //TODO();
  rtl_not(&id_dest->val);
  operand_write(id_dest,&id_dest->val);

  print_asm_template1(not);
}
