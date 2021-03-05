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
bool wp_is_full();//是否满
bool wp_is_empty();//是否空
WP* new_wp();//获取新节点
void _free_wp(WP* wp);//删除新节点通过wp
void free_wp(int num);//删除新节点通过编号
WP* finr_prev(WP* wp);//找到它的前一个
void print_wp_info();//打印已生成的检测点信息
void wp_append(WP*wp);//将生成的新节点追加在链表最后
void wp_check_change_and_print();//检测监视点是否有变化
#endif
