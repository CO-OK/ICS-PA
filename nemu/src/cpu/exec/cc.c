#include "cpu/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {//94
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O:
    case CC_B:{
      
    }
    case CC_E:{
      printf("t00=%d\n",t0);
      rtl_get_ZF(&t0);
      printf("t01=%d\n",t0);
      if(t0==0)//等于0说明ZF=0
      {
        *dest = 0;
      }
      else
      {
        *dest = 1;
      }
      break;
    }
    case CC_BE:
    case CC_S:
    case CC_L:
    case CC_LE:
      TODO();
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}
