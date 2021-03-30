#include <am.h>
#include <klib.h>

int main(){
  _ioe_init();
  int sec = 1;
  while (1) 
  {
    printf("uptime=%ud\n",_uptime());
    while(_uptime() < 100000000000 * sec) ;
    if (sec == 1) 
    {
      printf("%d second.\n", sec);
    } else 
    {
      printf("%d seconds.\n", sec);
    }
    //
    sec ++;
  }
  return 0;
}
