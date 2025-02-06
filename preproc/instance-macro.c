#include "stdio.h"

// void get_int(int *obj, void *ptr)
// {
//   *obj = *((int*)ptr);
// }

// void get_float(float *obj, void *ptr)
// {
//   *obj = *((float*)ptr);
// }


#define get_x(type) void get_ ## type(type *obj, void *ptr) { \
    *obj = *((type *)ptr);\
}

get_x(int);

int main(int argc, char const *argv[])
{
  int a;
  int b = 613;
  void *c = (void*)&b;

  get_int(&a, c);

  printf("%d\n", a);

  return 0;
}
