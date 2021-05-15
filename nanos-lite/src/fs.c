#include "fs.h"

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};
extern void fb_write(const void *buf, off_t offset, size_t count);
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
  file_table[FD_FB].size=_screen.width*_screen.height*4;
  Log("init_fs with size=%d",_screen.height*_screen.width*4);
}
int fs_open(const char*path,int flags,int mode)
{
  //printf("name=%s\n",path);
  
  for(int i=0;i<NR_FILES;i++)
  {
    if(strcmp(file_table[i].name,path)==0)
    {
      Log("open %s,fd=%d",path,i);
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
  Log("read %s fileSize=%d count=%d,open_offset=%d",file_table[fd].name,file_table[fd].size,count,file_table[fd].open_offset,((uint32_t*)(buf)));
  if(file_table[fd].open_offset >= fs_filesz(fd))
		return 0;
  if (file_table[fd].open_offset + count > fs_filesz(fd))
		count = file_table[fd].size - file_table[fd].open_offset;

  ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, count);

  file_table[fd].open_offset+=count;
  Log("fs_read return ,open_offset=%d",file_table[fd].open_offset);
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
  Log("lseek %d offset=%d,whence=%d,filesize=%d",fd,offset,whence,file_table[fd].size);
  if(whence==SEEK_CUR)
  {
    if ((offset + file_table[fd].open_offset >= 0) && (offset + file_table[fd].open_offset <= fs_filesz(fd))) 
    {
				file_table[fd].open_offset += offset;
				return file_table[fd].open_offset;
		}
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

    return file_table[fd].open_offset;
  }
  else if(whence==SEEK_END)
  {
    file_table[fd].open_offset = fs_filesz(fd) + offset;
		return file_table[fd].open_offset;
  }
  panic("lseek panic\n");
  return -1;
  //printf("hit out");
}

ssize_t fs_write(int fd, void *buf, size_t count)
{
  
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
      /*int row, col;
      int offset = file_table[FD_FB].open_offset;
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
      }*/
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
  Log("write %d name=%s count=%d",fd,file_table[fd].name,count);
  
  return count;
}