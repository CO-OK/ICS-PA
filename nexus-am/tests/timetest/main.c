#include <am.h>
#include <klib.h>

int main(){
  _ioe_init();
  int sec = 1;
  while (1) 
  {
    while(_uptime() < 1000 * sec) ;
    if (sec == 1) 
    {
      printf("%d second.\n", sec);
    } else 
    {
      printf("%d seconds.\n", sec);
    }
    printf("uptime=%d\n",_uptime());
    sec ++;
  }
  return 0;
}
