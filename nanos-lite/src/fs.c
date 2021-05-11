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
int fs_open(char*path,int flags,int mode)
{
  //printf("name=%s\n",path);
  
  for(int i=0;i<NR_FILES;i++)
  {
    if(strcmp(file_table[i].name,path)==0)
    {
      Log("open %s,fd=%d\n",path,i);
      return i;
      //printf("out\n");
    }
    //printf("str=%s\n",file_table[i].name);
  }
  panic("panic at fs_open\n");
  return -1;
}
ssize_t fs_read(int fd, void *buf, size_t count)
{
  //Log("read %s fileSize=%d count=%d,open_offset=%d\n",file_table[fd].name,file_table[fd].size,count,file_table[fd].open_offset,((uint32_t*)(buf)));
  assert(fd>=0&&fd<NR_FILES);
  if(fd<3)
  {
    Log("wrong fd");
    return 0;
  }
  int n= fs_filesz(fd)-file_table[fd].open_offset;
  if(n>count)
    n=count;
  ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, n);
  set_open_offset(fd,get_open_offset(fd)+n);
  return n;
  /*if(file_table[fd].open_offset >= fs_filesz(fd))
		return 0;
  if (file_table[fd].open_offset + count > fs_filesz(fd))
		count = file_table[fd].size - file_table[fd].open_offset;

  ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, count);

  file_table[fd].open_offset+=count;
  Log("fs_read return ,open_offset=%d",file_table[fd].open_offset);
  return count;*/
}

ssize_t fs_filesz(int fd)
{
  //printf("hit size\n");
  return file_table[fd].size;
}
int fs_close(int fd)
{
  assert(fd>=0&&fd<NR_FILES);
  Log("close %d\n",fd);
  //file_table[fd].open_offset=0;
  return 0;
}
off_t fs_lseek(int fd, off_t offset, int whence)
{
  Log("lseek %d offset=%d,whence=%d,filesize=%d\n",fd,offset,whence,file_table[fd].size);
  if(whence==SEEK_CUR)
  {
    if ((offset + file_table[fd].open_offset >= 0) && (offset + file_table[fd].open_offset <= fs_filesz(fd))) 
    {
				file_table[fd].open_offset += offset;
				return file_table[fd].open_offset;
		}

    //set_open_offset(fd,get_open_offset(fd)+offset);
    return file_table[fd].open_offset;
  }
  else if(whence==SEEK_SET)
  {
    if (offset >= 0 && offset <= fs_filesz(fd)) 
    {
			file_table[fd].open_offset =  offset;
			return offset;
		}
    else if(offset > fs_filesz(fd))
    {
      file_table[fd].open_offset=offset;
    }
    //set_open_offset(fd,offset);
    return file_table[fd].open_offset;
  }
  else if(whence==SEEK_END)
  {
    set_open_offset(fd,fs_filesz(fd)+offset);
		return file_table[fd].open_offset;
  }
  panic("lseek panic\n");
  return -1;
  //printf("hit out");
}

ssize_t fs_write(int fd, void *buf, size_t count)
{
  Log("write %d name=%s count=%d\n",fd,file_table[fd].name,count);
  if (file_table[fd].open_offset + count > fs_filesz(fd))
		count = file_table[fd].size - file_table[fd].open_offset;
  ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, count);
  file_table[fd].open_offset+=count;
  return count;
}
void set_open_offset(int fd , off_t n)
{
  assert(fd>=0&&fd<NR_FILES);
  assert(n>=0);
  if(n>file_table[fd].size)
    n=file_table[fd].size;
  file_table[fd].open_offset=n;
}
off_t get_open_offset(int fd)
{
  assert(fd>=0&&fd<NR_FILES);
  return file_table[fd].open_offset;
}