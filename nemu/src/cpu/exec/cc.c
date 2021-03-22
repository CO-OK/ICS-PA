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
  rtlreg_t temp0;
  rtlreg_t temp1;
  rtlreg_t temp2;
  switch (subcode & 0xe) {
    case CC_O:{//Jump short if overflow (OF=1)
      rtl_get_OF(&temp0);
      if(temp0==0)//等于0说明ZF=0
      {
        *dest = (unsigned char)0;
      }
      else
      {
        *dest = (unsigned char)1;
      }
      break;
    }
    case CC_B:{// Jump short if below (CF=1)
      rtl_get_CF(&temp0);
      if(temp0==0)//等于0说明ZF=0
      {
        *dest = (unsigned char)0;
      }
      else
      {
        *dest = (unsigned char)1;
      }
      break; 
    }
    case CC_E:{//Jump short if equal (ZF=1)
      rtl_get_ZF(&temp0);
      if(temp0==0)//等于0说明ZF=0
      {
        *dest = (unsigned char)0;
      }
      else
      {
        *dest = (unsigned char)1;
      }
      break;
    }
    case CC_BE:{// Jump short if not below or equal (CF=0 and ZF=0)
      rtl_get_ZF(&temp0);
      rtl_get_CF(&temp1);
      if(temp0==0&&temp1==0)
        *dest = (unsigned char)0;
      else
        *dest = (unsigned char)1;
      break;
    }
    case CC_S:{//Jump short if sign (SF=1)
      rtl_get_SF(&temp0);
      if(temp0==0)
        *dest = (unsigned char)0;
      else 
        *dest = (unsigned char)1;
      break;
    }
    case CC_L:{//Jump short if less (SF≠OF)
      rtl_get_SF(&temp0);
      rtl_get_OF(&temp1);
      if((temp0==0&&temp1!=0)||(temp0!=0&&temp1==0))
        *dest = (unsigned char)1;
      else 
        *dest = (unsigned char)0;
      break;
    }
    case CC_LE:{//Jump short if less or equal (ZF=1 and SF≠OF)
      rtl_get_SF(&temp0);
      rtl_get_OF(&temp1);
      rtl_get_ZF(&temp2);
      printf("temp0=%08Xtemp1=%08X\n",temp0,temp1);
      if(((temp0==0&&temp1!=0)||(temp0!=0&&temp1==0))/*&&temp2!=0*/)
        *dest = (unsigned char)1;
      else
        *dest = (unsigned char)0;
      break;
    }
      //TODO();
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}
