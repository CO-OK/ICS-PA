#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
  return 0;
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
  strncpy(buf, dispinfo + offset, len);
	return len;
}

void fb_write(const void *buf, off_t offset, size_t count) {
      int row, col;
      offset /= 4;
      col = offset % screen_width();
      row = offset / screen_width();
      int total = count/4;
      if(screen_width()-col>=total)//不止1行
      {
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
