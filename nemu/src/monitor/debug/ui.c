#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include <ctype.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
int is_digit(char*arg);
int is_digit_hex(char*arg);
int is_digit_hex(char*arg)
{
  if(strlen(arg)<=2)
    return 0;
  if(*arg!='0'||*(arg+1)!='x')
    return 0;
  char *p=arg+2;
  while(*p!='\0')
  {
    if(!isdigit(*arg)&&!(*p>=65&&*p<=70)&&!(*p>=97&&*p<=102))
    {
      return 0;
    } 
    p++;
  }
  return 1;
}
int is_digit(char*arg)
{
  while(*arg!='\0')
  {
    if(!isdigit(*arg))
    {
      return 0;
    } 
    arg++;
  }
  return 1;
}
/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

void bin2dec(uint32_t value, char *str)
{
  for(int i=0;i<33;i++)
  {
    str[i]='0';
  }
  int val=0;
  int left=0;
  str[32]='\0';
  for(int i=1;value!=0;i++)
  {
    val=value/2;
    left=value%2;
    value=val;
    str[32-i]=(char)(left+48);
  }
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}
static int cmd_si(char * args){
  char*arg=strtok(args," ");
  if(arg==NULL)
  {
    cpu_exec(1);
    return 0;
  }
  if(!is_digit(arg))
  {
     printf("arg must be a digit\n");
     return 0;
  }
  cpu_exec(atoi(arg)); 

  return 0;
}
static int cmd_info(char * args){
  char*arg=strtok(args," ");
  if(arg==NULL)
  {
    //打印所有寄存器状态
    printf("eax:%08X  ebx:%08X  ecx:%08X  edx:%08X\n",cpu.eax,cpu.ebx,cpu.ecx,cpu.edx);
    printf("esp:%08X  ebp:%08X  esi:%08X  edi:%08X\n",cpu.esp,cpu.ebp,cpu.esi,cpu.edi);
    printf("eip:%08X\n",cpu.eip);
    return 0;
  }
  while(arg!=NULL)
  {
    //打印所列出的寄存器的状态
    if(strcmp(arg,"eax")==0)
    {
      printf("eax:%08X\n",cpu.eax);
      arg=strtok(NULL," ");
      continue;
    }
    if(strcmp(arg,"ebx")==0)
    {
      printf("ebx:%08X\n",cpu.ebx);
      arg=strtok(NULL," ");
      continue;
    }
    if(strcmp(arg,"ecx")==0)
    {
      printf("ecx:%08X\n",cpu.ecx);
      arg=strtok(NULL," ");
      continue;
    }
    if(strcmp(arg,"edx")==0)
    {
      printf("edx:%08X\n",cpu.edx);
      arg=strtok(NULL," ");
      continue;
    }
    if(strcmp(arg,"esp")==0)
    {
      printf("esp:%08X\n",cpu.esp);
      arg=strtok(NULL," ");
      continue;
    }
    if(strcmp(arg,"ebp")==0)
    {
      printf("ebp:%08X\n",cpu.ebp);
      arg=strtok(NULL," ");
      continue;
    }
    if(strcmp(arg,"esi")==0)
    {
      printf("esi:%08X\n",cpu.esi);
      arg=strtok(NULL," ");
      continue;
    }
    if(strcmp(arg,"edi")==0)
    {
      printf("edi:%08X\n",cpu.edi);
      arg=strtok(NULL," ");
      continue;
    }
    if(strcmp(arg,"eip")==0)
    {
      printf("eip:%08X\n",cpu.eip);
      arg=strtok(NULL," ");
      continue;
    }
    printf("unknow arg:%s\n",arg);
    arg=strtok(NULL," ");
  }
  return 0; 
}

static int cmd_mem(char*args)
{
  char * addr=strtok(args," ");
  char * len=strtok(NULL," ");
  if(addr==NULL||len==NULL)
  {
    printf("arg wrong!\n usage: mem [addr] [len]\n");
    return 0;
  }
  if(!is_digit_hex(addr)||!is_digit(len))
  {
    printf("arg wrong!\naddr must be hex format.\nrange must be dec format.\n");
    return 0;
  }
  int addr_i; 
  for(int i=1;i<=atoi(len);i++)
  {
    addr_i=(unsigned int)strtol(addr,NULL,16)+i-1;
    printf("0x%X\t0x%X\n",addr_i,paddr_read(addr_i,1));
  }
  return 0;
}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "debug by step", cmd_si},
  { "info", "reg state for temp", cmd_info},
  { "mem",  "memary layout", cmd_mem},
  
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  printf("batch_mode_is:%d\n",is_batch_mode);
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    //printf("first token is %s\n",cmd);
    if (cmd == NULL) { continue;printf("continuing...\n"); }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
