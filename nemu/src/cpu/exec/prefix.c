#include "cpu/exec.h"

make_EHelper(real);

make_EHelper(operand_size) {
  decoding.is_operand_size_16 = true;
  printf("eip=%08X\n",*eip);
  exec_real(eip);
  decoding.is_operand_size_16 = false;
}

make_EHelper(xchg_b)
{
  
}