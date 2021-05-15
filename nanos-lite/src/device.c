#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};
extern int _read_key();
unsigned long _uptime();
size_t events_read(void *buf, size_t len) {
  Log("in events_read,len=%d",len);
  int key = _read_key();
  //通码的值为断码+0x8000
  //下面的代码摘自 keytest
  bool down = false;
  if ((key & 0x8000)!=0) {
    key ^= 0x8000;
    down = true;
  }
  unsigned long mytime =  _uptime();
  if(key!=_KEY_NONE)//优先处理按键
  {
    
    sprintf(buf, "%s %s\n", down ? "kd" : "ku", keyname[key]);
    //sprintf(buf,"Get key: %d %s %s\n", key, keyname[key], down ? "down" : "up");
  }
  else
  {
    sprintf(buf,"time=%u\n",mytime);
  }
  Log("return buf len=%d",strlen(buf));
  return strlen(buf);
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
  strncpy(buf, dispinfo + offset, len);

}
extern int screen_width();
extern int screen_height();
void fb_write(const void *buf, off_t offset, size_t count) {
      //Log("in  fb_write offset=%d,count=%d",offset,count);
      //Log("width=%d,height=%d",screen_width(),screen_height());
      int row, col;
      offset /= 4;
      col = offset % screen_width();
      row = offset / screen_width();
      int total = count/4;
      //Log("int fb_write col=%d,row=%d,total=%d",col,row,total);
      if(screen_width()-col<total)//不止1行
      {
        //Log("more than one line");
        //先写第一行
        _draw_rect((uint32_t *)buf, col, row, screen_width()-col, 1);
        total=total-(screen_width()-col);
        int full_line = total / screen_width();//可以写几个满行
        int left_line = total % screen_width();//最后1个不满的行所要写的数量
        //写满行的
        _draw_rect((uint32_t *)(buf+(screen_width()-col)*4), 0, row+1, screen_width(), full_line);
        //写最后一行
        _draw_rect((uint32_t *)(buf+(screen_width()-col)*4+full_line*screen_width()*4), 0, row+1+full_line, left_line, 1);
      }
      else
      {
        _draw_rect((uint32_t *)buf, col, row, total, 1);
      }
}

void init_device() {
  Log("init device");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", screen_width(), screen_height());
  
}
