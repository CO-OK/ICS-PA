#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB *current = NULL;

uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);
  uintptr_t entry = loader(&pcb[i].as, filename);
  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);
}
int count=0;
int current_game=0;//在0和2之间切换
_RegSet* schedule(_RegSet *prev) {
  /*if(prev!=NULL)
    current->tf=prev;
  current=(current==&pcb[0]?&pcb[1]:&pcb[0]);
  _switch(&current->as);
  return current->tf;*/
  if(prev!=NULL)
    current->tf=prev;
  else
    current=&pcb[current_game];
  if(current==&pcb[current_game])
  {
    count++;
    if(count==100)
    {
      //切换到hello
      current=&pcb[1];
      _switch(&current->as);
      count=0;
      return current->tf;
    }
    else
      return current->tf;
  }
  else
  {
    //直接切换
    current=&pcb[current_game];
    _switch(&current->as);
    return current->tf;
  }
}
