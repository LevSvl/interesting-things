#include "stdio.h"
#include "unistd.h"
#include "sys/wait.h"

int c = 0;

int main(int argc, char const *argv[])
{
  int pid;
  int a, b;

  a = 5;
  b = 10;

  pid = fork();

  if (pid == 0){
    b += 1;
    c += 2;
    printf("child:\ta = %d\tb = %d\tc = %d\n", a, b, c);
  }else{
    printf("parent:\ta = %d\tb = %d\tc = %d\n", a, b, c);
  }

  if (pid > 0){
    wait(0);
    printf("parent:\ta = %d\tb = %d\tc = %d\n", a, b, c);
  }
  
  return 0;
}
