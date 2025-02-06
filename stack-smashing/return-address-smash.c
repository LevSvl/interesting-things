#include "stdio.h"

int foo();

int main(int argc, char const *argv[])
{ 
  int a = 15;
  printf("a = %d\n", a);
  
  foo();

  int b = 20;
  printf("b = %d\n", b);

  return 0;
}

int foo()
{
  int arr[2];
  
  arr[0] = 3; 
  arr[1] = 2;
  arr[2] = main;

  return 1;
}