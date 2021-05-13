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
  rtl_and(&t0,&id_dest->val,&id_src->val);
  rtl_unset_CF(&eflag_CF);
  rtl_unset_OF(&eflag_OF);
  rtl_update_ZFSF(&t0,id_dest->width);
  print_asm_template2(test);
}

make_EHelper(and) {
  //TODO();
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

make_EHelper(xor) {
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
  rtl_shr(&t2, &id_dest->val, &id_src->val);
	operand_write(id_dest, &t2);
	rtl_update_ZFSF(&t2, id_dest->width);
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

make_EHelper(rol) {
	rtl_shl(&t0, &id_dest->val, &id_src->val);
	if (decoding.is_operand_size_16) {
		t3 = 0;
		rtl_addi(&t1, &t3, 16);
		rtl_sub(&t1, &t1, &id_src->val);
		rtl_shr(&t2, &id_dest->val, &t1);
	} else {
		t3 = 0;
		rtl_addi(&t1, &t3, 32);
		rtl_sub(&t1, &t1, &id_src->val);
		rtl_shr(&t2, &id_dest->val, &t1);
	}
	rtl_or(&t0, &t0, &t2);
	operand_write(id_dest, &t0);

	// unnecessary to update CF and OF in NEMU
	rtl_update_ZFSF(&t0, id_dest->width);

	print_asm_template2(shl);
}