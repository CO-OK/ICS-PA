#include <am.h>
#include <x86.h>
#include <stdio.h>
#define RTC_PORT 0x48   // Note that this is not standard
static unsigned long boot_time;
int min(int a,int b)
{
  if(a<b)
    return a;
  return b;
}
void _ioe_init() {
  boot_time = inl(RTC_PORT);
  //printf("time=%d\n",boot_time);
}

unsigned long _uptime() {
  unsigned long now_time=inl(RTC_PORT);
  //printf("boottime=%d\n",boot_time);
  return (now_time-boot_time);
}

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
  /*
    用于将 pixels 指定的矩形像素绘制到屏幕中以(x, y)和(x+w, y+h)两点连线为对角线的矩形区域
  */
  /*int i;
  for (i = 0; i < _screen.width * _screen.height; i++) {
    fb[i] = i;
  }*/
  for(int i=0;i<w;i++)
  {
    for(int j=0;j<h;j++)
    {
      fb[_screen.width*(y+j)+x+j]=*pixels;
      //pixels+=w;
    }
    pixels+=w;
  }

}

void _draw_sync() {
  //return;
}

int _read_key() {
  //用于返回按键的键盘码,若无按键,则返回_KEY_NONE
  /*
    i8042 初始化时会注册 0x60 处的端口作为数据寄存器,注册 0x64 处的端口作为状态寄存器
  */
  uint32_t is_key_event = inl(0x64);
  if(is_key_event)
  {
    return inl(0x60);
  }
  else
    return _KEY_NONE;
}
