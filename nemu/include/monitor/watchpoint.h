#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  int is_busy;
  /* TODO: Add more members if necessary */
  char expr[32];

} WP;
bool wp_is_full();
bool wp_is_empty();
WP* new_wp();
void _free_wp(WP* wp);
void free_wp(int num);
WP* finr_prev(WP* wp);
#endif
