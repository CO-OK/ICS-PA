#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //printf("val=%08X\n",id_dest->val);
  /*
    push imm8 指令需要对立即数进行符号扩展
  */
  //printf("opcode=%08X\n",decoding.opcode);
  //if(decoding.opcode)
  if(id_dest->width == 1){
	  	uint8_t utmp = id_dest->val;
		int8_t temp = utmp;
		id_dest->val = temp;
	}
  rtl_push(&id_dest->val,4);  

  print_asm_template1(push);
}


make_EHelper(pop) {
  rtl_pop(&t0);
	if(id_dest->width == 1){
		uint8_t utemp = t0;
		int8_t temp = utemp; 
		id_dest->val = temp;
	}
	else 
	  id_dest->val = t0;
  operand_write(id_dest, &id_dest->val);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  //TODO();
  Log("begin puhsa esp=%08X",cpu.esp);
  rtlreg_t temp=cpu.esp;
  rtl_push(&cpu.eax,4);
  rtl_push(&cpu.ecx,4);
  rtl_push(&cpu.edx,4);
  rtl_push(&cpu.ebx,4);
  rtl_push(&temp,4);
  rtl_push(&cpu.ebp,4);
  rtl_push(&cpu.esi,4);
  rtl_push(&cpu.edi,4);
  Log("finish pusha esp=%08X",cpu.esp);
  //printf("pusha %08X\n",cpu.esp);
  print_asm("pusha");
}

make_EHelper(popa) {
  //TODO();
  //printf("before popa %08X\n",cpu.esp);
  Log("begin popa esp=%08X",cpu.esp);
  rtlreg_t temp;
  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&temp);
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);
  Log("finish popa esp=%08X",cpu.esp);
  //printf("popa %08X\n",cpu.esp);
  print_asm("popa");
}

make_EHelper(leave) {
  //TODO();
  rtl_mv(&cpu.esp,&cpu.ebp);
  rtl_pop(&cpu.ebp);
  
  print_asm("leave");
}

make_EHelper(cltd) {
  //printf("width=%d\n",id_dest->width);
  if (decoding.is_operand_size_16) {
		rtl_sext(&t0, &reg_l(R_EAX), 2);
		rtl_shri(&reg_l(R_EDX), &t0, 16);
	}
	else {
		rtl_sari(&reg_l(R_EDX), &reg_l(R_EAX), 31);
	}

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
		rtl_shli(&reg_l(R_EAX), &reg_l(R_EAX), 24);
		rtl_sari(&reg_l(R_EAX), &reg_l(R_EAX), 8);
	  rtl_shri(&reg_l(R_EAX), &reg_l(R_EAX), 16);
	}
	else {
		rtl_sext(&reg_l(R_EAX), &reg_l(R_EAX), 2);
	} 
	print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
	rtl_sext(&t0, &id_src->val, id_src->width);
	operand_write(id_dest, &t0);
	print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
	operand_write(id_dest, &id_src->val);
	print_asm_template2(movzx);
}

make_EHelper(lea) {
  
  rtl_li(&t2, id_src->addr);
  //printf("width=%d\n",id_dest->width);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
