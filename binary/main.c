#include <stdio.h>


extern char _binary_img_start[];

int main(int argc, char const *argv[])
{
  // char *a = _binary_img_start;
  printf("img size %x\n", *_binary_img_start);
  return 0;
}
