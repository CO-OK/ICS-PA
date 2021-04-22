#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  /*
    目前的 loader 只需要做一件事情:将 ramdisk 中从 0 开始的所有内容放置在 0x4000000,并把这个地
    址作为程序的入口返回即可
  */
  ramdisk_read(DEFAULT_ENTRY,0,get_ramdisk_size());
  printf("load end\n");
  return (uintptr_t)DEFAULT_ENTRY;
}
