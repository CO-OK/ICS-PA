#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <stdio.h>
#include <sys/types.h>
#include <regex.h>
#include <math.h>
uint32_t eval(int head,int tail);
int find_domin(int head,int tail);
int hex2dec(char *str);
int power( int x, int n );
uint32_t get_reg_value(char*reg);
bool check_parentheses(int head,int tail);
enum {
  TK_NOTYPE = 256, 
  TK_EQ,
  TK_DIGIT,
  TK_DIGIT_HEX,
  TK_LEFT_SMALL_BRACE,
  TK_RIGHT_SMALL_BRACE,
  TK_ADD_SUB,
  TK_MUL_DIV,
  TK_REG,
  TK_GREAT_LESS,//> < >= <=
  TK_ADDR,//*取值
  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", TK_ADD_SUB},         // plus
  {"-",TK_ADD_SUB},            //minis
  {"\\*",TK_MUL_DIV},          //mul
  {"/",TK_MUL_DIV},            //div
  {"==", TK_EQ},         // equal
  {"\\(",TK_LEFT_SMALL_BRACE}, //左括号
  {"\\)",TK_RIGHT_SMALL_BRACE},//右括号
  {">=",TK_GREAT_LESS},
  {"<=",TK_GREAT_LESS},
  {">",TK_GREAT_LESS},              //大于号
  {"<",TK_GREAT_LESS},               //小于号
  {"0x([0-9]|[A-F]|[a-f])+",TK_DIGIT_HEX},//十六进制数字
  {"[0-9]+",TK_DIGIT},//十进制数字
  {"\\$[a-z]+",TK_REG},//寄存器
  
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        if(rules[i].token_type!=TK_NOTYPE)
        {
          int j=0;
          for(;j<substr_len;j++)
          {
            tokens[nr_token].str[j]=*(substr_start+j);
          }
          tokens[nr_token].str[j]='\0';
          if(rules[i].token_type==TK_MUL_DIV&&rules[i].regex[0]=='\\')//是取地址运算
          {
            if(nr_token==0||(tokens[nr_token-1].type!=TK_DIGIT_HEX&&tokens[nr_token-1].type!=TK_DIGIT&&tokens[nr_token-1].type!=TK_REG))
            {
              tokens[nr_token].type=TK_ADDR;
            }
          }
          else
          {
            tokens[nr_token].type=rules[i].token_type;
          }
          //printf("%s\n",tokens[nr_token].str);
          nr_token++;
          break;
        }
        else{
          //
        }
        
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //printf("%d\n",eval(0,nr_token-1));

  return eval(0,nr_token-1);
}
uint32_t eval(int head,int tail)
{
  if(head>tail)
  {
    return -1;
  }
  else if(head==tail)
  {
    if(tokens[head].type==TK_DIGIT)
      return atoi(tokens[head].str);
    if(tokens[head].type==TK_DIGIT_HEX)
    {
      //int a=hex2dec(tokens[head].str);
      return hex2dec(tokens[head].str);
    }
    if(tokens[head].type==TK_REG)
      return get_reg_value(tokens[head].str);
    
  }
  else if(check_parentheses(head,tail))
  {
    return eval(head+1,tail-1);
  }
  else
  {
    int op=find_domin(head, tail);
    if(tokens[op].type!=TK_ADDR)
    {
      int val1=eval(head,op-1);
      int val2=eval(op+1,tail);
      printf("val1=%d,val2=%d\n",val1,val2);
      switch(tokens[op].str[0])
      {
        case '+':{
          return val1+val2;
        }
        case '-':{
          return val1-val2;
        }
        case '*':{
          return val1*val2;
        }
        case '/':{
          return val1/val2;
        }
        case '>':{
          if(tokens[op].str[1]=='\0')
            return val1>val2;
          else if(tokens[op].str[1]=='=')
            return val1>=val2;
        }
        case '<':{
          if(tokens[op].str[1]=='\0')
            return val1<val2;
          else if(tokens[op].str[1]=='=') 
            return val1<=val2;
        }
        case '=':{
          if(tokens[op].str[1]=='=')
            return val1==val2;
          break;
        }
        default:
          assert(0);
      }
    }
    else
    {
      int val;
      if(tokens[op+1].type==TK_DIGIT)
        val=atoi(tokens[op+1].str);
      if(tokens[op+1].type==TK_DIGIT_HEX)
      {
        val=hex2dec(tokens[op+1].str);
      }
      printf("val=%d\n",val);
      return paddr_read(val,1);
    }
    
  }
  return -1;
}

bool check_parentheses(int head,int tail)
{
  return tokens[head].type==TK_LEFT_SMALL_BRACE&&tokens[tail].type==TK_RIGHT_SMALL_BRACE;
}
int find_domin(int head,int tail)
{
  int current_domin=-1;
  for(int i=head;i<=tail;i++)
  {
    if(tokens[i].type!=TK_ADD_SUB&&tokens[i].type!=TK_MUL_DIV&&tokens[i].type!=TK_LEFT_SMALL_BRACE&&tokens[i].type!=TK_RIGHT_SMALL_BRACE&&\
    tokens[i].type!=TK_GREAT_LESS&&tokens[i].type!=TK_EQ&&tokens[i].type!=TK_ADDR)//不是运算符，继续
      continue;
    if((tokens[i].type==TK_MUL_DIV||tokens[i].type==TK_ADD_SUB||tokens[i].type==TK_GREAT_LESS\
    ||tokens[i].type==TK_EQ||tokens[i].type==TK_ADDR)&&current_domin==-1)//第一次出现的运算符
    {
      current_domin=i;
      continue;
    }
    switch(tokens[i].type)
    {
      case TK_ADDR:{
        if(tokens[current_domin].type==TK_ADDR)
          current_domin=i;
      }
      case TK_MUL_DIV:{
        if(tokens[current_domin].type==TK_MUL_DIV||tokens[current_domin].type==TK_ADDR)
          current_domin=i;
        break;
      }
      case TK_ADD_SUB:{
        if(tokens[current_domin].type==TK_MUL_DIV||tokens[current_domin].type==TK_ADD_SUB||tokens[current_domin].type==TK_ADDR)
          current_domin=i;
        break;
      }
      case TK_GREAT_LESS:{
        if(tokens[current_domin].type==TK_MUL_DIV||tokens[current_domin].type==TK_ADD_SUB||tokens[current_domin].type==TK_GREAT_LESS||tokens[current_domin].type==TK_ADDR)
          current_domin=i;
        break;
      }
      case TK_EQ:{
        current_domin=i;
        break;
      }
      case TK_LEFT_SMALL_BRACE:{
        int new_pos=i;
        while(tokens[new_pos].type!=TK_RIGHT_SMALL_BRACE){new_pos++;}
        i=new_pos;
        break;
      }
    }
  }
  return current_domin;
}

int hex2dec(char *str)
{
  int len=strlen(str)-2;
  int total=0;
  for(int i=0;i<len;i++)
  {
    if(str[len+1-i]>='0'&&str[len+1-i]<='9')
      total+=(str[len+1-i]-48)*power(16,i);
    else if(str[len+1-i]>='A'&&str[len+1-i]<='F')
      total+=(str[len+1-i]-55)*power(16,i);
    else if(str[len+1-i]>='a'&&str[len+1-i]<='f')
      total+=(str[len+1-i]-87)*power(16,i);
  }
  return total;
}

int power( int x, int n ) 
{ 
  int i; 
  int s=1; 
  for( i=1;i<=n;i++ )
    s*=x; 
  return s;
} 

uint32_t get_reg_value(char*reg)
{
    char*arg=reg+1;
    if(strcmp(arg,"eax")==0)
    {
      return cpu.eax;
    }
    if(strcmp(arg,"ebx")==0)
    {
      return cpu.ebx;
    }
    if(strcmp(arg,"ecx")==0)
    {
      return cpu.ecx;
    }
    if(strcmp(arg,"edx")==0)
    {
      return cpu.edx;
    }
    if(strcmp(arg,"esp")==0)
    {
      return cpu.esp;
    }
    if(strcmp(arg,"ebp")==0)
    {
      return cpu.ebp;
    }
    if(strcmp(arg,"esi")==0)
    {
      return cpu.esi;
    }
    if(strcmp(arg,"edi")==0)
    {
      return cpu.edi;
    }
    if(strcmp(arg,"eip")==0)
    {
      return cpu.eip;
    }
    printf("unknow reg:%s\n",arg);
    return -1;
}