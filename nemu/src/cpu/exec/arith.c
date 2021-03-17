#include "cpu/exec.h"

make_EHelper(add) {

  //TODO();
  printf("dest_type=%d\nsrc_type=%d\n",id_dest->type,id_src->type);
  
  get_mr_value(&t0,id_dest);
  get_mr_value(&t1,id_src);
  rtl_add(&t2,&t1,&t0);
  operand_write(id_dest,&t2);
  /*
    OF, SF, ZF, AF\, CF, and PF\ as described in Appendix C
  */
  rtl_update_ZFSF(&t2,id_dest->width);
  /*
    OF标志位根据操作数的符号及其变化情况来设置：若两个操作数的符号相同，而结果的符号与之相反时，OF=1，否则OF=0
  */
  rtlreg_t op1,op2,res;
  rtl_msb(&op1,&t0,id_dest->width);
  rtl_msb(&op2,&t1,id_src->width);
  rtl_msb(&res,&t2,id_dest->width);
  if(op1==op2&&res!=op1)
    rtl_set_OF(&eflag_OF);
  else
    rtl_unset_OF(&eflag_OF);
  if(id_dest->width==1)
  {
    op1=INT8_MAX-t1;
  }
  else if(id_dest->width==2)
  {
    op1=INT16_MAX-t1;
  }
  else if(id_dest->width==4)
  {
    op1=INT32_MAX-t1;
  }
  op2=t0;
  rtl_sltu(&res,&op1,&op2);
  if(res)
    rtl_set_CF(&eflag_CF);
  else
    rtl_unset_CF(&eflag_CF);
  //rtl_sltu()
  print_asm_template2(add);
}

make_EHelper(sub) {
  //TODO();
  //目前只支持reg-reg、reg-imm
  rtl_sub(&t2, &id_dest->val, &id_src->val);//t2存结果
  rtl_sltu(&t3, &id_dest->val, &id_src->val);//t3=1 ---> val<t2 other wise val>=t2  which mean cf=1
  operand_write(id_dest, &t2);//结果写入相应寄存器
  //The sub instruction is used to perform a substraction. It modifies the 2 following flags: ZF (Zero Flag) and CF (Carry Flag). 
  //OF, SF\, ZF\, AF\, PF\, and CF as described in Appendix C ， nemu不管pf af
  rtl_update_ZFSF(&t2, id_dest->width);
  
  if(t3)//cf==0
  {
    rtl_set_CF(&eflag_CF);
  } 
    //rtl_unset_CF(&eflag_CF);
  else
    rtl_unset_CF(&eflag_CF);
  /*
    OF:
    减法的OF位的设置方法为：若两个数的符号相反，而结果的符号与减数的符号相同，则OF=1，除上述情况外OF=0。OF=1说明带符号数的减法运算结果是错误的。
  */
  rtl_shli(&t0,&id_dest->val,id_dest->width * 8 - 1);
  rtl_shli(&t1,&id_src->val,id_src->width * 8 -1);
  rtl_shli(&t3,&t2,id_src->width * 8 -1);
  if((t0 ^ t1)&&(!(t3 ^ t1)))//两个数的符号相反，而结果的符号与减数的符号相同
  {
    rtl_set_OF(&eflag_OF);
  }
  else
    rtl_unset_OF(&eflag_OF);


  print_asm_template2(sub);

}

make_EHelper(cmp) {
  TODO();

  print_asm_template2(cmp);
}

make_EHelper(inc) {
  TODO();

  print_asm_template1(inc);
}

make_EHelper(dec) {
  TODO();

  print_asm_template1(dec);
}

make_EHelper(neg) {
  TODO();

  print_asm_template1(neg);
}

make_EHelper(adc) {
  rtl_add(&t2, &id_dest->val, &id_src->val);
  rtl_sltu(&t3, &t2, &id_dest->val);
  rtl_get_CF(&t1);
  rtl_add(&t2, &t2, &t1);
  operand_write(id_dest, &t2);

  rtl_update_ZFSF(&t2, id_dest->width);

  rtl_sltu(&t0, &t2, &id_dest->val);
  rtl_or(&t0, &t3, &t0);
  rtl_set_CF(&t0);

  rtl_xor(&t0, &id_dest->val, &id_src->val);
  rtl_not(&t0);
  rtl_xor(&t1, &id_dest->val, &t2);
  rtl_and(&t0, &t0, &t1);
  rtl_msb(&t0, &t0, id_dest->width);
  rtl_set_OF(&t0);

  print_asm_template2(adc);
}

make_EHelper(sbb) {
  rtl_sub(&t2, &id_dest->val, &id_src->val);
  rtl_sltu(&t3, &id_dest->val, &t2);
  rtl_get_CF(&t1);
  rtl_sub(&t2, &t2, &t1);
  operand_write(id_dest, &t2);

  rtl_update_ZFSF(&t2, id_dest->width);

  rtl_sltu(&t0, &id_dest->val, &t2);
  rtl_or(&t0, &t3, &t0);
  rtl_set_CF(&t0);

  rtl_xor(&t0, &id_dest->val, &id_src->val);
  rtl_xor(&t1, &id_dest->val, &t2);
  rtl_and(&t0, &t0, &t1);
  rtl_msb(&t0, &t0, id_dest->width);
  rtl_set_OF(&t0);

  print_asm_template2(sbb);
}

make_EHelper(mul) {
  rtl_lr(&t0, R_EAX, id_dest->width);
  rtl_mul(&t0, &t1, &id_dest->val, &t0);

  switch (id_dest->width) {
    case 1:
      rtl_sr_w(R_AX, &t1);
      break;
    case 2:
      rtl_sr_w(R_AX, &t1);
      rtl_shri(&t1, &t1, 16);
      rtl_sr_w(R_DX, &t1);
      break;
    case 4:
      rtl_sr_l(R_EDX, &t0);
      rtl_sr_l(R_EAX, &t1);
      break;
    default: assert(0);
  }

  print_asm_template1(mul);
}

// imul with one operand
make_EHelper(imul1) {
  rtl_lr(&t0, R_EAX, id_dest->width);
  rtl_imul(&t0, &t1, &id_dest->val, &t0);

  switch (id_dest->width) {
    case 1:
      rtl_sr_w(R_AX, &t1);
      break;
    case 2:
      rtl_sr_w(R_AX, &t1);
      rtl_shri(&t1, &t1, 16);
      rtl_sr_w(R_DX, &t1);
      break;
    case 4:
      rtl_sr_l(R_EDX, &t0);
      rtl_sr_l(R_EAX, &t1);
      break;
    default: assert(0);
  }

  print_asm_template1(imul);
}

// imul with two operands
make_EHelper(imul2) {
  rtl_sext(&id_src->val, &id_src->val, id_src->width);
  rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);

  rtl_imul(&t0, &t1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t1);

  print_asm_template2(imul);
}

// imul with three operands
make_EHelper(imul3) {
  rtl_sext(&id_src->val, &id_src->val, id_src->width);
  rtl_sext(&id_src2->val, &id_src2->val, id_src->width);
  rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);

  rtl_imul(&t0, &t1, &id_src2->val, &id_src->val);
  operand_write(id_dest, &t1);

  print_asm_template3(imul);
}

make_EHelper(div) {
  switch (id_dest->width) {
    case 1:
      rtl_li(&t1, 0);
      rtl_lr_w(&t0, R_AX);
      break;
    case 2:
      rtl_lr_w(&t0, R_AX);
      rtl_lr_w(&t1, R_DX);
      rtl_shli(&t1, &t1, 16);
      rtl_or(&t0, &t0, &t1);
      rtl_li(&t1, 0);
      break;
    case 4:
      rtl_lr_l(&t0, R_EAX);
      rtl_lr_l(&t1, R_EDX);
      break;
    default: assert(0);
  }

  rtl_div(&t2, &t3, &t1, &t0, &id_dest->val);

  rtl_sr(R_EAX, id_dest->width, &t2);
  if (id_dest->width == 1) {
    rtl_sr_b(R_AH, &t3);
  }
  else {
    rtl_sr(R_EDX, id_dest->width, &t3);
  }

  print_asm_template1(div);
}

make_EHelper(idiv) {
  rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);

  switch (id_dest->width) {
    case 1:
      rtl_lr_w(&t0, R_AX);
      rtl_sext(&t0, &t0, 2);
      rtl_msb(&t1, &t0, 4);
      rtl_sub(&t1, &tzero, &t1);
      break;
    case 2:
      rtl_lr_w(&t0, R_AX);
      rtl_lr_w(&t1, R_DX);
      rtl_shli(&t1, &t1, 16);
      rtl_or(&t0, &t0, &t1);
      rtl_msb(&t1, &t0, 4);
      rtl_sub(&t1, &tzero, &t1);
      break;
    case 4:
      rtl_lr_l(&t0, R_EAX);
      rtl_lr_l(&t1, R_EDX);
      break;
    default: assert(0);
  }

  rtl_idiv(&t2, &t3, &t1, &t0, &id_dest->val);

  rtl_sr(R_EAX, id_dest->width, &t2);
  if (id_dest->width == 1) {
    rtl_sr_b(R_AH, &t3);
  }
  else {
    rtl_sr(R_EDX, id_dest->width, &t3);
  }

  print_asm_template1(idiv);
}
