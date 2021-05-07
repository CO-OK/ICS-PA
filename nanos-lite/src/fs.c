#include "fs.h"

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin (note that this is not the actual stdin)", 0, 0},
  {"stdout (note that this is not the actual stdout)", 0, 0},
  {"stderr (note that this is not the actual stderr)", 0, 0},
  [FD_FB] = {"/dev/fb", 0, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}
int fs_open(char*path)
{
  for(int i=0;i<NR_FILES;i++)
  {
    if(strcmp(file_table[i].name,path)==0)
      return i;
  }
  return -1;
}
int fs_read(int fd, void *buf, size_t count)
{
  ramdisk_read(buf, file_table[fd].disk_offset, count);
}

ssize_t fs_filesz(int fd)
{
  return file_table[fd].size;
}
int fs_close(int fd)
{
  return 0;
}