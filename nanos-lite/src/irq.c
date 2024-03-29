#include "common.h"

static _RegSet* do_event(_Event e, _RegSet* r) {
  //printf("event ID = %d\n",e.event);
  switch (e.event) {
    //
    case _EVENT_SYSCALL:{
      return do_syscall(r);
      //printf("return schdelu\n");
      //return schedule(r);
      break;
    }
    case _EVENT_TRAP:{
      //Log("hit _EVENT_TRAP");
      return schedule(r);
      break;
    }
    case _EVENT_IRQ_TIME:{
      //Log("hit _EVENT_IRQ_TIME");
      return schedule(r);
      break;
    }
    default: panic("Unhandled event ID = %d", e.event);
  }
  return NULL;
}

void init_irq(void) {
  _asye_init(do_event);
}
