#include "common.h"
#include "fs.h"
//#define DEFAULT_ENTRY ((void *)0x4000000)
#define DEFAULT_ENTRY ((void *)0x8048000)
uintptr_t loader(_Protect *as, const char *filename) {
  int fd = fs_open(filename,0,0);
  size_t file_size = fs_filesz(fd);
  size_t page_num = file_size / PGSIZE;
  if(file_size%PGSIZE!=0)
    page_num++;
  void* pa;
  void* va = (void*)DEFAULT_ENTRY;
  for(int i = 0; i < page_num; i ++){
    pa = new_page();
    _map(as, va, pa);
    fs_read(as,va,pa);
    va += PGSIZE;
  }
  fs_close(fd);
  return DEFAULT_ENTRY;
}
