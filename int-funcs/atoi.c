#include "stdio.h"
#include "string.h"

int atoi(char *str){
  int res = 0;

  while(*str >= '0' && *str <= '9')
    res = res*10 + *str++ - '0';
  return res;
}

int main(int argc, char const *argv[])
{
  char *s = "12243";
  printf("%d\n", atoi(s));
  return 0;
}
