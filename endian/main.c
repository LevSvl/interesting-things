#include "stdio.h"
#include "stdint.h"

int main(int argc, char const *argv[])
{
  int16_t i;

  i = 1;

  if (*((int8_t*)&i))
    printf("little endian\n");
  else
    printf("big endian\n");

  return 0;
}
