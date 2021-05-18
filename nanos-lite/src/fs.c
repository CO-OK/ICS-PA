#include "fs.h"

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};
extern void fb_write(const void *buf, off_t offset, size_t count);
void dispinfo_read(void *buf, off_t offset, size_t len);
size_t events_read(void *buf, size_t len);
extern int screen_width();

extern int screen_height();

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
  file_table[FD_FB].size=screen_width()*screen_height()*4;
  Log("init_fs with size=%d",screen_height()*screen_width()*4);
}
int fs_open(const char*path,int flags,int mode)
{
  //printf("name=%s\n",path);
  
  for(int i=0;i<NR_FILES;i++)
  {
    if(strcmp(file_table[i].name,path)==0)
    {
      //Log("open %s,fd=%d",path,i);
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
  //Log("read %s fileSize=%d count=%d,open_offset=%d",file_table[fd].name,file_table[fd].size,count,file_table[fd].open_offset,((uint32_t*)(buf)));
  switch(fd){
    case FD_DISPINFO:{
      if(file_table[fd].open_offset >= fs_filesz(fd))
        return 0;
      if (file_table[fd].open_offset + count > fs_filesz(fd))
        count = file_table[fd].size - file_table[fd].open_offset;
			dispinfo_read(buf, file_table[fd].open_offset, count);
			file_table[fd].open_offset += count;	
			break;
    }
    case FD_EVENTS:{
      count=events_read(buf, count);
      break;
    }
    default :{
      if(file_table[fd].open_offset >= fs_filesz(fd))
        return 0;
      if (file_table[fd].open_offset + count > fs_filesz(fd))
        count = file_table[fd].size - file_table[fd].open_offset;

      ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, count);

      file_table[fd].open_offset+=count;
    }
  }
  
  //Log("fs_read return ,open_offset=%d",file_table[fd].open_offset);
  return count;
}

ssize_t fs_filesz(int fd)
{
  //printf("hit size\n");
  return file_table[fd].size;
}
int fs_close(int fd)
{
  
  Log("close %s",file_table[fd].name);
  //file_table[fd].open_offset=0;
  return 0;
}
off_t fs_lseek(int fd, off_t offset, int whence)
{
  //Log("lseek %d offset=%d,whence=%d,filesize=%d",fd,offset,whence,file_table[fd].size);
  if(whence==SEEK_CUR)
  {
    if ((offset + file_table[fd].open_offset >= 0) && (offset + file_table[fd].open_offset <= fs_filesz(fd))) 
    {
      //Log("lseek in CUR");
			file_table[fd].open_offset += offset;
			return file_table[fd].open_offset;
		}
  
  }
  else if(whence==SEEK_SET)
  {
    if (offset >= 0 && offset <= fs_filesz(fd)) 
    {
      //Log("lseek in SET");
			file_table[fd].open_offset =  offset;
      //Log("open offset=%d",offset);
			return offset;
		}
    /*else if(offset > fs_filesz(fd))
    {
      file_table[fd].open_offset=offset;
    }*/

    //return file_table[fd].open_offset;
  }
  else if(whence==SEEK_END)
  {
    //Log("lseek in END");
    file_table[fd].open_offset = fs_filesz(fd) + offset;
		return file_table[fd].open_offset;
  }
  //panic("lseek panic\n");
  return -1;
}

ssize_t fs_write(int fd, void *buf, size_t count)
{
  //Log("write %d name=%s count=%d",fd,file_table[fd].name,count);
  switch(fd){
    case FD_STDOUT:
    case FD_STDERR:{
      for(int i=0;i<count;i++)
      {
        _putc(((char*)buf)[i]);
        //
      }
      break;
    }
    case FD_FB:{
      //void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h)
      //每一个元素占 4 个字节
      if (file_table[fd].open_offset + count > fs_filesz(fd))
		    count = file_table[fd].size - file_table[fd].open_offset;
      fb_write(buf,file_table[fd].open_offset,count);
      file_table[fd].open_offset+=count;
      break;
    }
    default:{
      if (file_table[fd].open_offset + count > fs_filesz(fd))
		    count = file_table[fd].size - file_table[fd].open_offset;
      ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, count);
      file_table[fd].open_offset+=count;
    }
  }
  
  
  return count;
}