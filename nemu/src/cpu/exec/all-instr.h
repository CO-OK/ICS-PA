#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);
make_EHelper(call);
make_EHelper(push_reg);
make_EHelper(pop_reg);
make_EHelper(sub);
make_EHelper(xor_31);
make_EHelper(ret);
make_EHelper(lea);
make_EHelper(and);
make_EHelper(nop);
make_EHelper(add);
make_EHelper(cmp);
make_EHelper(setcc);
make_EHelper(movzx);
make_EHelper(test);
make_EHelper(jcc);
make_EHelper(inc);
make_EHelper(jmp);
make_EHelper(dec);
make_EHelper(imul2);
make_EHelper(sar);
make_EHelper(shl);
make_EHelper(or);
make_EHelper(leave);
make_EHelper(not);
make_EHelper(cltd);
make_EHelper(idiv);
make_EHelper(movsx);
make_EHelper(adc);
make_EHelper(jmp_rm);
make_EHelper(imul1);
make_EHelper(call_rm);
make_EHelper(shr);
make_EHelper(mul);
make_EHelper(sbb);
make_EHelper(div);
make_EHelper(in);
make_EHelper(out);
//make_EHelper(xchg_b);
//make_EHelper(push_Ev);