#include "common.h"

static _RegSet* do_event(_Event e, _RegSet* r) {
  printf("event ID = %d\n",e.event);
  if(e.event==_EVENT_SYSCALL)
  {
     switch (e.event) {
    //default: panic("Unhandled event ID = %d", e.event);
      default : do_syscall(r);
    }
  }
  return NULL;
}

void init_irq(void) {
  _asye_init(do_event);
}
