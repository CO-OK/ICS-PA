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
}

void fb_write(const void *buf, off_t offset, size_t count) {
      int row, col;
      offset /= 4;
      col = offset % _screen.width;
      row = offset / _screen.width;
      int total = count/4;
      if(_screen.width-col>=total)//不止1行
      {
        //先写第一行
        _draw_rect((uint32_t *)buf, col, row, _screen.width-col, 1);
        total=total-(_screen.width-col);
        int full_line = total / _screen.width;//可以写几个满行
        int left_line = total % _screen.width;//最后1个不满的行所要写的数量
        //写满行的
        _draw_rect((uint32_t *)(buf+(_screen.width-col)*4), 0, row+1, _screen.width, full_line);
        //写最后一行
        _draw_rect((uint32_t *)(buf+(_screen.width-col)*4+full_line*_screen.width*4), 0, row+1+full_line, left_line, 1);
      }
      else
      {
        _draw_rect((uint32_t *)buf, col, row, total, 1);
      }
}

void init_device() {
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
