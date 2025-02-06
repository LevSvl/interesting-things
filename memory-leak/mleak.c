#include "stdlib.h"

volatile void foo()
{
  int a = 67;
  int b = 65;
  volatile int *c = (int*)malloc(10*sizeof(int));
  volatile int *d = (int*)malloc(10*sizeof(int));
  volatile int *e = (int*)malloc(10*sizeof(int));
}

int main(int argc, char const *argv[])
{
  int a = 5;
  int b = 7;
  volatile int *c = (int*)malloc(10*sizeof(int));
  
  foo();

  volatile int *r = (int*)malloc(10*sizeof(int));
  volatile int *l = (int*)malloc(10*sizeof(int));
  
  free((void*)c);
  
  return 0;
}
