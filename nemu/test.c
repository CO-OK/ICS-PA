#include<stdio.h>
#include<string.h>
void bin2dec(int value, char *str)
{
  for(int i=0;i<32;i++)
  {
    str[i]='0';
  }
  int val=0;
  int left=0;
  str[31]='\0';
  for(int i=1;value!=0;i++)
  {
    val=value/2;
    left=value%2;
    value=val;
    str[31-i]=(char)(left+48);
    printf("left=%d\n",left);
    printf("val=%d\n",val);
    printf("%c\n",str[i]);

  }
}
int main()
{
		char str[32];
    char s[5]="lll";
		bin2dec(10,str);
		printf("%s\n",str);
    printf("%d\n",strlen(str));
		return 0;
}
