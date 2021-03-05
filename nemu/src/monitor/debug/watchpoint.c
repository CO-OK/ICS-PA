#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    //wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i].next=NULL;
    wp_pool[i].is_busy=0;
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}
WP* new_wp()
{
  for(int i=0;i<NR_WP;i++)
  {
    if(wp_pool[i].is_busy==0)
    {
      wp_pool[i].is_busy=1;
      return &wp_pool[i];
    }
      
  }
  return NULL;
}
WP* finr_prev(WP* wp)
{
  for(int i=0;i<NR_WP;i++)
  {
    if(wp_pool[i].next==wp)
      return &wp_pool[i];
  }
  return NULL;
}
void free_wp(WP* wp)
{
  if(wp==head)
  {
    head=wp->next;
    
  }
  else if(wp->next==NULL)
  {
    finr_prev(wp)->next=NULL;
  }
  else
  {
    WP*prev=finr_prev(wp);
    prev->next=wp->next;
  }
  wp->next=NULL;
  wp->is_busy=0;
}

bool wp_is_full()
{
  for(int i=0;i<NR_WP;i++)
  {
    if(wp_pool[i].is_busy==0)
      return false;
  }
  return 1;
}

bool wp_is_empty()
{
  for(int i=0;i<NR_WP;i++)
  {
    if(wp_pool[i].is_busy==1)
      return false;
  }
  return true;
}
/* TODO: Implement the functionality of watchpoint */


