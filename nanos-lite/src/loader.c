#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  //printf("before load\n");
  int fd = fs_open("/bin/hello", 0, 0);
	fs_read(fd, (void *)DEFAULT_ENTRY, fs_filesz(fd));
	fs_close(fd);
	return DEFAULT_ENTRY;
  //ramdisk_read(DEFAULT_ENTRY,0,get_ramdisk_size());
  return (uintptr_t)DEFAULT_ENTRY;
}
