#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  int is_busy;
  /* TODO: Add more members if necessary */


} WP;
bool wp_is_full();
bool wp_is_empty();
WP* new_wp();
void free_wp(WP* wp);
WP* finr_prev(WP* wp);
#endif
