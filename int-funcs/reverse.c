#include "stdio.h"
#include "limits.h"
#include "stdlib.h"

int reverse1(int x)
{
  int rem = 0;
  int cdig = 0;
  int res = 0;
  int s = 1;
  if(x < 0) s = -1;
  unsigned int xt = abs(x);
  do {
    if (res > INT_MAX/10) return 0;
    res = res * 10; // здесь дописали защи ту от переполнения
    cdig = xt % 10;
    xt = (xt - cdig) / 10;
    res = res + cdig;
  }
  while (xt != 0);
  return s * res;
}

int reverse(int x)
{
  int sign = 1;
  if (x < 0) sign = -1;
  unsigned int ux = abs(x);
  int ost = 0;
  int res = 0;

  while(ux != 0){
    ost = ux % 10;

    if(INT_MAX/10 < res || INT_MIN/10 > res)
      return 0;
    res = res*10;
    
    if(INT_MAX-ost < res || INT_MIN+ost > res)
      return 0;  
    res += ost;
      

    ux = (ux-ost)/10;
  }
  return (sign * res);
}

int main(int argc, char const *argv[])
{

  // int max 2147483647 -> 214748364->7463847412 
  int x = 7463847412;
  printf("%d\n", reverse(232039203));
  return 0;
}
