#include "fs.h"

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;
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
  //printf("name=%s\n",path);
  printf("open %s\n",path);
  int i;
  for(i=0;i<NR_FILES;i++)
  {
    if(strcmp(file_table[i].name,path)==0)
    {
      //printf("hit %s\n",path);
      return i;
      //printf("out\n");
    }
    //printf("str=%s\n",file_table[i].name);
  }
  assert(0);
  return -1;
}
int fs_read(int fd, void *buf, size_t count)
{
  printf("read %d size=%d\n",fd,count);
  if(file_table[fd].open_offset == fs_filesz(fd))
		return 0;
  if (file_table[fd].open_offset + count > fs_filesz(fd))
		count = file_table[fd].size - file_table[fd].open_offset;
  ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, count);
  file_table[fd].open_offset+=count;
  printf("read out\n");
  return count;
}

ssize_t fs_filesz(int fd)
{
  //printf("hit size\n");
  return file_table[fd].size;
}
int fs_close(int fd)
{
  printf("close %d\n",fd);
  return 0;
}
off_t lseek(int fd, off_t offset, int whence)
{
  printf("lseek %d offset=%d,whence=%d,filesize=%d\n",fd,offset,whence,file_table[fd].size);
  if(whence==SEEK_CUR)
  {
    if ((offset + file_table[fd].open_offset >= 0) && (offset + file_table[fd].open_offset <= fs_filesz(fd))) 
    {
				file_table[fd].open_offset += offset;
				return file_table[fd].open_offset;
			}
  }
  else if(whence==SEEK_SET)
  {
    if (offset >= 0 && offset <= fs_filesz(fd)) 
    {
			file_table[fd].open_offset =  offset;
			return offset;
		}
  }
  else if(whence==SEEK_END)
  {
    file_table[fd].open_offset = fs_filesz(fd) + offset;
		return file_table[fd].open_offset;
  }
  //printf("hit out");
}

int sys_write(int fd, char *buf, size_t count)
{
  //printf("write %d count=%d\n",fd,count);
  //printf("fd=%d\ncount=%d\n",fd,count);
  if(fd==1||fd==2)
  {
    for(int i=0;i<count;i++)
    {
      _putc(buf[i]);
      //printf("char=%c\n",buf[i]);
    }
    
    return count;
  }
  else
  {
    if (file_table[fd].open_offset + count > fs_filesz(fd))
		  count = file_table[fd].size - file_table[fd].open_offset;
    ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, count);
    file_table[fd].open_offset+=count;
    return count;
  }
  return -1;
}
