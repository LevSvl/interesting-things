#include "stdio.h"

int var = 0;

void foo() __attribute__((constructor));
void bar() __attribute__((destructor));

void foo()
{
  var = 1;
}

void bar()
{
  printf("destructor: exit\n");
}

int main(int argc, char const *argv[])
{
  printf("var = %d\n", var);
  return 0;
}
