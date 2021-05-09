#include "cpu/exec.h"

//void rtl_mul(rtlreg_t* dest_hi, rtlreg_t* dest_lo, const rtlreg_t* src1, const rtlreg_t* src2);
make_EHelper(add) {
  rtl_add(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest,&t2);
  rtl_update_ZFSF(&t2,id_dest->width);
  rtl_sltu(&t0,&t2,&id_dest->val);
  if(t0!=0)
    rtl_set_CF(&eflag_CF);
  else
    rtl_unset_CF(&eflag_CF);
  rtl_xor(&t0,&id_src->val,&t2);
  rtl_xor(&t1,&id_dest->val,&t2);
  rtl_and(&t0,&t0,&t1);
  rtl_msb(&t0,&t0,id_dest->width);
  if(t0!=0)
    rtl_set_OF(&eflag_OF);
  else
    rtl_unset_OF(&eflag_OF);
  
  print_asm_template2(add);
}

make_EHelper(sub) {
  //TODO();
  //目前只支持reg-reg、reg-imm
  /*rtl_sub(&t2, &id_dest->val, &id_src->val);//t2存结果
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
  
  rtl_shri(&t0,&id_dest->val,id_dest->width * 8 - 1);
  rtl_shri(&t1,&id_src->val,id_dest->width * 8 -1);
  rtl_shri(&t3,&t2,id_dest->width * 8 -1);
  if((t0 ^ t1)&&(!(t3 ^ t1)))//两个数的符号相反，而结果的符号与减数的符号相同
  {
    rtl_set_OF(&eflag_OF);
  }
  else
    rtl_unset_OF(&eflag_OF);
*/
  rtl_sext(&t1, &id_dest->val, id_dest->width);
	rtl_sext(&t2, &id_src->val, id_src->width);

	rtl_sub(&t0, &t1, &t2);
	t3 = (t0 > t1);
  if(t3!=0)
    rtl_set_CF(&eflag_CF);
  else
    rtl_unset_CF(&eflag_CF);
	t3 = ((((int32_t)(t1) < 0) == (((int32_t)(t2) >> 31) == 0)) && (((int32_t)(t0) < 0) != ((int32_t)(t1) < 0))); // 负正得正 正负得负
  if(t3!=0)
    rtl_set_OF(&eflag_OF);
  else
    rtl_unset_OF(&eflag_OF);
	rtl_update_ZFSF(&t0, 4);
	operand_write(id_dest, &t0);
  print_asm_template2(sub);

}

make_EHelper(cmp) {
  //TODO();
  /*
    CMP (Compare) subtracts the source operand from the destination operand. It
    updates OF, SF, ZF, AF, PF, and CF but does not alter the source and
    destination operands. A subsequent Jcc or SETcc instruction can test the
    appropriate flags.
    The CF, OF, SF, ZF, AF, and PF flags are set according to the result
    If an operand greater than one byte is compared to an
    immediate byte, the byte value is first sign-extended.

  */
  rtl_sext(&t1, &id_dest->val, id_dest->width);
	rtl_sext(&t2, &id_src->val, id_src->width);

	rtl_sub(&t0, &t1, &t2);
	t3 = (t0 > t1);
  if(t3!=0)
    rtl_set_CF(&eflag_CF);
  else
    rtl_unset_CF(&eflag_CF);
	t3 = ((((int32_t)(t1) < 0) == (((int32_t)(t2) >> 31) == 0)) && (((int32_t)(t0) < 0) != ((int32_t)(t1) < 0))); // 负正得正 正负得负
  if(t3!=0)
    rtl_set_OF(&eflag_OF);
  else
    rtl_unset_OF(&eflag_OF);
	rtl_update_ZFSF(&t0, 4);
  print_asm_template2(cmp);
}

make_EHelper(inc) {
  //TODO();
  /*
    可能有问题
  */
  rtl_addi(&t2,&id_dest->val,1);
  operand_write(id_dest,&t2);
  rtl_update_ZFSF(&t2,id_dest->width);
  rtl_eqi(&t0,&t2,0x80000000);
  if(t0!=0)
    rtl_set_OF(&eflag_OF);
  else
    rtl_unset_OF(&eflag_OF);
  print_asm_template1(inc);
}

make_EHelper(dec) {
  //TODO();
  //printf("val=%08X\n",id_dest->val);
  rtl_subi(&t0,&id_dest->val,1);
  operand_write(id_dest,&t0);
  //rtl_sr(id_dest->reg,id_dest->width,&t0);
  rtl_update_ZFSF(&t0,id_dest->width);  
  rtl_eqi(&t2,&t0,0x7fffffff);
  if(t2!=0)
    rtl_set_OF(&eflag_OF);
  else
    rtl_unset_OF(&eflag_OF);
  print_asm_template1(dec);
}

make_EHelper(neg) {
  //TODO();
  rtlreg_t t =0;
  rtl_sub(&t2,&t,&id_dest->val);
  rtl_update_ZFSF(&t2,id_dest->width);
  rtl_neq0(&t0,&id_dest->val);
  if(t0!=0)
    rtl_set_CF(&eflag_CF);
  else
    rtl_unset_CF(&eflag_CF);
  rtl_eqi(&t0,&id_dest->val,0x80000000);
  if(t0!=0)
    rtl_set_OF(&eflag_OF);
  else
    rtl_unset_OF(&eflag_OF);
  operand_write(id_dest,&t2);
  print_asm_template1(neg);
}

make_EHelper(adc) {
  rtl_add(&t2, &id_dest->val, &id_src->val);
  rtl_sltu(&t3, &t2, &id_dest->val);
  rtl_get_CF(&t1);
  if(t1!=0)
    t1=1;
  rtl_add(&t2, &t2, &t1);
  operand_write(id_dest, &t2);

  rtl_update_ZFSF(&t2, id_dest->width);

  rtl_sltu(&t0, &t2, &id_dest->val);
  rtl_or(&t0, &t3, &t0);
  if(t0!=0)
    rtl_set_CF(&eflag_CF);
  else
    rtl_unset_CF(&eflag_CF);
  rtl_xor(&t0, &id_dest->val, &id_src->val);
  rtl_not(&t0);
  rtl_xor(&t1, &id_dest->val, &t2);
  rtl_and(&t0, &t0, &t1);
  rtl_msb(&t0, &t0, id_dest->width);
  if(t0!=0)
    rtl_set_OF(&eflag_OF);
  else
    rtl_unset_OF(&eflag_OF);

  print_asm_template2(adc);
}

make_EHelper(sbb) {
  rtl_sub(&t2, &id_dest->val, &id_src->val);
  rtl_sltu(&t3, &id_dest->val, &t2);
  rtl_get_CF(&t1);
  if(t1!=0)
    t1=1;
  rtl_sub(&t2, &t2, &t1);
  operand_write(id_dest, &t2);

  rtl_update_ZFSF(&t2, id_dest->width);

  rtl_sltu(&t0, &id_dest->val, &t2);
  rtl_or(&t0, &t3, &t0);
  if(t0!=0)
    rtl_set_CF(&eflag_CF);
  else
    rtl_unset_CF(&eflag_CF);

  rtl_xor(&t0, &id_dest->val, &id_src->val);
  rtl_xor(&t1, &id_dest->val, &t2);
  rtl_and(&t0, &t0, &t1);
  rtl_msb(&t0, &t0, id_dest->width);
  if(t0!=0)
    rtl_set_OF(&eflag_OF);
  else
    rtl_unset_OF(&eflag_OF);
  //printf("dest=%08X\nsrc=%08X\n",id_dest->val,id_src->val);
  /*rtl_sub(&t2, &id_dest->val, &id_src->val);
  rtl_sltu(&t3, &id_dest->val, &t2);
  rtl_get_CF(&t1);
  //printf("t1=%08X\n",t1);
  rtlreg_t temp;
  if(t1!=0)//cf=1
  {
    rtl_subi(&temp, &t2, 1);
  }
  else
  {
    rtl_subi(&temp,&t2,0);
  }
  operand_write(id_dest, &temp);

  rtl_update_ZFSF(&temp, id_dest->width);

  rtl_sltu(&t0, &t2, &temp);
  //printf("t3=%08X\n,t0=%08X\n",t3,t0);
  rtl_or(&t0, &t3, &t0);
  if(t0==1)
  {
    rtl_set_CF(&eflag_CF);
  }
  else
  {
    rtl_unset_CF(&eflag_CF);
  }

  rtl_xor(&t0, &id_dest->val, &id_src->val);
  rtl_xor(&t1, &id_dest->val, &temp);
  rtl_and(&t0, &t0, &t1);
  rtl_msb(&t0, &t0, id_dest->width);
  t0 &= 0x00000001;
  if(t0==0)
  {
    rtl_unset_OF(&eflag_OF);
  }
  else
  {
    rtl_set_OF(&eflag_OF);
  }*/
  print_asm_template2(sbb);
}

make_EHelper(mul) {
  rtl_lr(&t0, R_EAX, id_dest->width);
  rtl_mul(&t0, &t1, &id_dest->val, &t0);
  rtlreg_t temp;
  switch (id_dest->width) {
    case 1:{
      /*
        The carry and overflow flags are set to 0 if AH is 0; otherwise, they are set to 1.
      */  
      rtl_sr_w(R_AX, &t1);
      temp = cpu.eax & 0x0000ff00;
      if(temp==0)
      {
        rtl_unset_CF(&eflag_CF);
        rtl_unset_OF(&eflag_OF);
      }
      else
      {
        rtl_set_CF(&eflag_CF);
        rtl_set_OF(&eflag_OF);
      }
      break;
    }  
    case 2:{
      /*
        The carry and overflow flags are set to 0 if DX is 0; otherwise, they are set to 1.
      */
      rtl_sr_w(R_AX, &t1);
      rtl_shri(&t1, &t1, 16);
      rtl_sr_w(R_DX, &t1);
      temp = cpu.edx & 0x0000ffff;
      if(temp==0)
      {
        rtl_unset_CF(&eflag_CF);
        rtl_unset_OF(&eflag_OF);
      }
      else
      {
        rtl_set_CF(&eflag_CF);
        rtl_set_OF(&eflag_OF);
      }
      break;
    } 
    case 4:{
      rtl_sr_l(R_EDX, &t0);
      rtl_sr_l(R_EAX, &t1);
      /*
        The carry and overflow flags are set to 0 if EDX is 0; otherwise, they are set to 1.
      */
     if(cpu.eax==0)
     {
        rtl_unset_CF(&eflag_CF);
        rtl_unset_OF(&eflag_OF);
     }
     else
     {
        rtl_set_CF(&eflag_CF);
        rtl_set_OF(&eflag_OF);
     }
      break;
    }
      
    default: assert(0);
  }

  print_asm_template1(mul);
}

// imul with one operand
make_EHelper(imul1) {
  rtl_lr(&t0, R_EAX, id_dest->width);
  rtl_imul(&t0, &t1, &id_dest->val, &t0);
  //printf("777\n");
  /*
    Instruction Form Condition for Clearing CF and OF
    r/m8
    AL = sign-extend of AL to 16 bits
    r/m16
    AX = sign-extend of AX to 32 bits
    r/m32
    EDX:EAX = sign-extend of EAX to 32 bits
    
    The CF and OF flags are set when significant bits are carried into the
    high-order half of the result. CF and OF are cleared when the
    high-order half of the result is the sign-extension of the low-order half.

    imul sets SF according to the sign bit of the truncated result
  */
  rtlreg_t temp;
  //printf("width=%d\n",id_dest->width);
  switch (id_dest->width) {
    case 1:{
      rtl_sr_w(R_AX, &t1);
      rtl_sext(&temp,&cpu.eax,1);
      temp = temp & 0x0000ffff;
      rtl_lr(&t2,R_EAX,id_dest->width);
      if(temp==t2)
      {
        rtl_unset_CF(&eflag_CF);
        rtl_unset_OF(&eflag_OF);
      }
      else
      {
        rtl_set_CF(&eflag_CF);
        rtl_set_OF(&eflag_OF);
      }
      rtl_msb(&temp,&cpu.eax,1);
      temp &= 0x00000001;
      if(temp!=0)
      {
        rtl_set_SF(&eflag_SF);
      }
      else
        rtl_unset_SF(&eflag_SF);
      break;
    }  
    case 2:
    {
      
      rtl_sr_w(R_AX, &t1);
      rtl_shri(&t1, &t1, 16);
      rtl_sr_w(R_DX, &t1);
      rtl_sext(&temp,&cpu.eax,2);
      //temp = temp & 0xffffffff;
      rtl_lr(&t2,R_EAX,id_dest->width);
      if(temp==t2)
      {
        rtl_unset_CF(&eflag_CF);
        rtl_unset_OF(&eflag_OF);
      }
      else
      {
        rtl_set_CF(&eflag_CF);
        rtl_set_OF(&eflag_OF);
      }
      rtl_msb(&temp,&cpu.eax,2);
      temp &= 0x00000001;
      if(temp!=0)
      {
        rtl_set_SF(&eflag_SF);
      }
      else
        rtl_unset_SF(&eflag_SF);
      break;
    }
    case 4:
    {
      temp=cpu.edx;
      rtl_sr_l(R_EDX, &t0);
      rtl_sr_l(R_EAX, &t1);
      if(cpu.edx==temp)
      {
        rtl_unset_CF(&eflag_CF);
        rtl_unset_OF(&eflag_OF);
      }
      else
      {
        rtl_set_CF(&eflag_CF);
        rtl_set_OF(&eflag_OF);
      }
      rtl_msb(&temp,&cpu.eax,4);
      temp &= 0x00000001;
      if(temp!=0)
      {
        rtl_set_SF(&eflag_SF);
      }
      else
        rtl_unset_SF(&eflag_SF);
      break;
    }
      
    default: assert(0);
  }

  print_asm_template1(imul);
}

// imul with two operands
make_EHelper(imul2) {
  rtl_sext(&id_src->val, &id_src->val, id_src->width);
  rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);
  //printf("dest=%08X\nsrc=%08X\nwidth=%d\n\n",id_dest->val,id_src->val,id_dest->width);
  t0=0;
  rtl_imul(&t0, &t1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t1);
  //printf("t0=%08X\nt1=%08X\n",t0,t1);
  /*
    Clearing CF and OF when:
    r16,r/m16   Result exactly fits within r16
    r/32,r/m32  Result exactly fits within r32
  */
  //rtl_update_ZFSF(&t1,id_dest->width);
  rtlreg_t temp;
  if(id_dest->width==2)
  {
    temp = t1 & 0xffff0000;
    if(temp==0)
    {
      rtl_unset_OF(&eflag_OF);
      rtl_unset_CF(&eflag_CF);
    }
    else
    {
      rtl_set_CF(&eflag_CF);
      rtl_set_OF(&eflag_OF);
    }
  }
  else if(id_dest->width==4)
  {
    if(t0==0)
    {
      rtl_unset_OF(&eflag_OF);
      rtl_unset_CF(&eflag_CF);
    }
    else
    {
      rtl_set_CF(&eflag_CF);
      rtl_set_OF(&eflag_OF);
    }
  }
  rtl_unset_ZF(&eflag_ZF);
  rtl_msb(&t3,&t1,id_dest->width);
  t3 &= 0x00000001;
  if(t3==1)
    rtl_set_SF(&eflag_SF);
  else
    rtl_unset_SF(&eflag_SF);
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
make_EHelper(das){
  rtlreg_t a=cpu.eax;
  a=a&0x000000ff;
  if(a&0xf>9)
  {
    a=a-6;
    cpu.eax=cpu.eax&0xffffff00+a;
  }
  a=cpu.eax&0x000000ff;
  if(a>0x9f||eflag_CF==true)
  {
    a=a-0x60;
    cpu.eax=cpu.eax&0xffffff00+a;
    rtl_set_CF(&eflag_CF);
  }
  else
  {
    rtl_unset_CF(&eflag_CF);
  }
}

