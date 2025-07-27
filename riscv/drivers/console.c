#include <stdarg.h>

#include "drivers/uart.h"

 __attribute__ ((aligned (32))) static char digits[] = "0123456789abcdef";

static int 
convert(unsigned long x, int base, char buf[16])
{
  int i;

  i = 0;
  do{
    buf[i++] = digits[x % base];
  } while ( (x /= base) > 0);

  return i;
}

static void 
printint(int n, int base, int sign)
{
  char buf[16];
  int i;
  unsigned x;

  if (sign && (sign = n < 0))
    x = -n;
  else
    x = n;
  
  i = 0;
  i = convert(x, base, buf);
  
  if (sign)
    buf[i++] = '-';

  while (--i >= 0)
    uartputc(buf[i]);
}

static void
printptr(unsigned long n)
{
  int i;
  char buf[16];

  i = convert(n, 16, buf); 

  uartputc('0');
  uartputc('x');

  while (--i >= 0)
    uartputc(buf[i]);
}

void 
printf(char* fmt, ...)
{
  va_list ap;
  char* p;

  union fmtval
  {
    int ival; // целое
    double bval; // дробь
    char* sval; // символ
    unsigned long ptrval; // указатель
  } fmtval;
  

  va_start(ap, fmt);
  for(p = fmt; *p ; p++){
    if (*p != '%'){
      uartputc(*p);
      continue;
    }

    switch (*++p)
    {
    case 'd':
      fmtval.ival = va_arg(ap, int);
      printint(fmtval.ival, 10, 1);
      break;
    case 'f':
      break;
    case 'x':
      fmtval.ival = va_arg(ap, int);
      printint(fmtval.ival, 16, 1);
      break;
    case 's':
      break;
    case 'p':
      fmtval.ptrval = va_arg(ap, unsigned long);
      printptr(fmtval.ptrval);
      break;
    }
  }
  va_end(ap);
}
