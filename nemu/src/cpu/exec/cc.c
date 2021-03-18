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
  rtlreg_t temp0=t0;
  rtlreg_t temp1=t1;
  rtlreg_t temp2=t2;
  switch (subcode & 0xe) {
    case CC_O:{//Jump short if overflow (OF=1)
      rtl_get_OF(&t0);
      if(t0==0)//等于0说明ZF=0
      {
        *dest = (char)0;
      }
      else
      {
        *dest = (char)1;
      }
      break;
    }
    case CC_B:{// Jump short if below (CF=1)
      rtl_get_CF(&t0);
      if(t0==0)//等于0说明ZF=0
      {
        *dest = (char)0;
      }
      else
      {
        *dest = (char)1;
      }
      break; 
    }
    case CC_E:{//Jump short if equal (ZF=1)
      rtl_get_ZF(&t0);
      if(t0==0)//等于0说明ZF=0
      {
        *dest = (char)0;
      }
      else
      {
        *dest = (char)1;
      }
      break;
    }
    case CC_BE:{// Jump short if not below or equal (CF=0 and ZF=0)
      rtl_get_ZF(&t0);
      rtl_get_CF(&t1);
      if(t0==0&&t1==0)
        *dest = (char)0;
      else
        *dest = (char)1;
      break;
    }
    case CC_S:{//Jump short if sign (SF=1)
      rtl_get_SF(&t0);
      if(t0==0)
        *dest = (char)0;
      else 
        *dest = (char)1;
      break;
    }
    case CC_L:{//Jump short if less (SF≠OF)
      rtl_get_SF(&t0);
      rtl_get_OF(&t1);
      if(t0!=t1)
        *dest = (char)1;
      else 
        *dest = (char)0;
      break;
    }
    case CC_LE:{//Jump short if less or equal (ZF=1 and SF≠OF)
      rtl_get_SF(&t0);
      rtl_get_OF(&t1);
      rtl_get_ZF(&t2);
      if(t0!=t1&&t2==1)
        *dest = (char)1;
      else
        *dest = (char)0;
    }
      //TODO();
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
  t0=temp0;
  t1=temp1;
  t2=temp2;
}
