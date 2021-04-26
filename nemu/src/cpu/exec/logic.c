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
  get_mr_value(&t1,id_dest);
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
  get_mr_value(&t1,id_dest);
  get_mr_value(&t2,id_src);
  rtl_or(&t0,&t1,&t2);
  operand_write(id_dest,&t0);
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
  if(id_src->val==0)
  {
    print_asm_template2(sar);
    return;
  }
  rtl_msb(&t3,&id_dest->val,id_dest->width);
  rtl_shri(&t1,&id_dest->val,id_src->val-1);//先右移n-1位，留最后一位填充CF
  t2 = t1 & 0x00000001;

  if(t2 == 1)
    rtl_set_CF(&eflag_CF);
  else
    rtl_unset_CF(&eflag_CF);  
  rtl_shr(&t1,&id_dest->val,&id_src->val);
  //补1
  if(t3 == 1)//需要补0
  {
    rtlreg_t digit = 0;//用这个数与t1相加来补1
    for(int i = 0;i<id_src->val;i++)
    {
      digit += my_power(2,id_dest->width*8-1-i);
    }
    if(id_dest->width==1)
    {
      t1 += (unsigned char)digit;
    } 
    else if(id_dest->width==2)
    {
      t1 += (short)digit;
    }
    else
    {
      t1 += digit;
    }
  }
  operand_write(id_dest,&t1);
  rtl_unset_OF(&eflag_OF);
  rtl_update_ZFSF(&t1,id_dest->width);
  // unnecessary to update CF and OF in NEMU
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

  if(id_src->val==0)
  {
    print_asm_template2(shl);
    return;
  }
  rtl_shli(&t0,&id_dest->val,id_src->val-1);//留1位来给CF
  //这种类型的指令只有16位和32位

  if(id_dest->width==2)
  {
    t2 = t0 & 0x8000;
    if(t2 == 0x8000)//说明最高位是1
      rtl_set_CF(&eflag_CF);
    else
      rtl_unset_CF(&eflag_CF);
  }
  else if(id_dest->width==4)
  {
    t2 = t0 & 0x80000000;
    if(t2 == 0x80000000)//说明最高位是1
      rtl_set_CF(&eflag_CF);
    else
      rtl_unset_CF(&eflag_CF);
  }
  rtl_shl(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0,id_dest->width);
  //OF: OF ← high-order bit of r/m ≠ (CF);
  rtl_msb(&t1,&id_dest->val,id_dest->width);
  if(t1!= e_CF)
    rtl_set_OF(&eflag_OF);
  else
    rtl_unset_OF(&eflag_OF);
  print_asm_template2(shl);
}

make_EHelper(shr) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  if(id_src->val==0)
  {
    print_asm_template2(sar);
    return;
  }
  rtl_msb(&t3,&id_dest->val,id_dest->width);
  rtl_shri(&t1,&id_dest->val,id_src->val-1);//先右移n-1位，留最后一位填充CF
  t2 = t1 & 0x00000001;

  if(t2 == 1)
    rtl_set_CF(&eflag_CF);
  else
    rtl_unset_CF(&eflag_CF);  
  rtl_shr(&t1,&id_dest->val,&id_src->val);

  operand_write(id_dest,&t1);
  rtl_unset_OF(&eflag_OF);
  rtl_update_ZFSF(&t1,id_dest->width);

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
  get_mr_value(&t0,id_dest);
  t0=~t0;
  operand_write(id_dest,&t0);
  print_asm_template1(not);
}
