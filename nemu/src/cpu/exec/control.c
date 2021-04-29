#include "cpu/exec.h"

make_EHelper(jmp) {
  // the target address is calculated at the decode stage
  decoding.is_jmp = 1;
  decoding.tmp_eip=decoding.seq_eip;
  print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  decoding.is_jmp = t2;

  print_asm("j%s %x", get_cc_name(subcode), decoding.jmp_eip);
}

make_EHelper(jmp_rm) {
  //printf("val=%08X\n",id_dest->val);
  //printf("jmp_eip=%08X\n",decoding.jmp_eip);
  decoding.jmp_eip = id_dest->val;
  decoding.is_jmp = 1;

  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
  // the target address is calculated at the decode stage
  //TODO();
  decoding.is_jmp=1; 
  decoding.is_call=true;
  rtl_push(&decoding.seq_eip,4);
  //printf("ret addr")
  print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
  //TODO();
  /*if(decoding.is_call==true)
  {
    rtl_pop(&decoding.jmp_eip);
    decoding.is_call=false;
    decoding.is_jmp=1; 
  }
  else
  {
    decoding.jmp_eip=decoding.tmp_eip;
    decoding.is_jmp=1;
  }*/
  //printf("%08X\n",decoding.jmp_eip);
   rtl_pop(&decoding.jmp_eip);
   decoding.is_jmp=1; 
  //rtl_j(decoding.jmp_eip);
  print_asm("ret");
}

make_EHelper(call_rm) {
  //TODO();
  //printf("val=%08X\n",id_dest->val);
  decoding.is_jmp=1; 
  decoding.is_call=true;
  rtl_push(&decoding.seq_eip,4);
  decoding.jmp_eip=id_dest->val;
  print_asm("call *%s", id_dest->str);
}
