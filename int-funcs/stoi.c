#include "stdio.h"

char* stoi(int n, char *res)
{
  int buf[31];
  int sign = (n < 0) ? 1: 0;
  int u = abs(n);
  int i = 0;

  while(u != 0){
    buf[i++] = u % 10;  
    u /= 10;
  }
  i -= 1;

  int j = 0;
  if(sign){
    res[0] = '-';
    j = 1;
  }

  while(i >= 0){
    res[j++] = '0' + buf[i--];
  }

  return res;
}


int main(int argc, char const *argv[])
{
  int x = -5443;
  char res[32];
  printf("%s\n", stoi(x, res));

  return 0;
}
