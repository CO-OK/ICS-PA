#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <stdio.h>
#include <sys/types.h>
#include <regex.h>
int eval(int head,int tail);
int find_domin(int head,int tail);
bool check_parentheses(int head,int tail);
enum {
  TK_NOTYPE = 256, 
  TK_EQ,
  TK_DIGIT,
  TK_LEFT_SMALL_BRACE,
  TK_RIGHT_SMALL_BRACE,
  TK_ADD_SUB,
  TK_MUL_DIV,

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
  {"[0-9]+",TK_DIGIT},//十进制数字
  
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
        int j=0;
        for(;j<substr_len;j++)
        {
          tokens[nr_token].str[j]=*(substr_start+j);
        }
        tokens[nr_token].str[j]='\0';
        tokens[nr_token].type=rules[i].token_type;
        //printf("%s\n",tokens[nr_token].str);
        nr_token++;
        break;
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
  printf("%d\n",eval(0,nr_token-1));

  return 0;
}
int eval(int head,int tail)
{
  if(head>tail)
  {
    return -1;
  }
  else if(head==tail)
  {
    if(tokens[head].type==TK_DIGIT)
      return atoi(tokens[head].str);
  }
  else if(check_parentheses(head,tail))
  {
    return eval(head+1,tail-1);
  }
  else
  {
    int op=find_domin(head, tail);
    int val1=eval(head,op-1);
    int val2=eval(op+1,tail);
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
      default:
        assert(0);
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
    if(tokens[i].type!=TK_ADD_SUB&&tokens[i].type!=TK_MUL_DIV&&tokens[i].type!=TK_LEFT_SMALL_BRACE&&tokens[i].type!=TK_RIGHT_SMALL_BRACE)
      continue;
    if((tokens[i].type==TK_MUL_DIV||tokens[i].type==TK_ADD_SUB)&&current_domin==-1)
    {
      current_domin=i;
      continue;
    }
    switch(tokens[i].type)
    {
      case TK_ADD_SUB:{
        current_domin=i;
        break;
      }
      case TK_MUL_DIV:{
        current_domin=i;
        break;
      }
      case TK_LEFT_SMALL_BRACE:{
        int new_pos=i;
        while(tokens[new_pos].type!=TK_RIGHT_SMALL_BRACE){new_pos++;}
        i=new_pos+1;
        break;
      }
    }
  }
  return current_domin;
}