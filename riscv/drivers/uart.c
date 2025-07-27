#define UART0 0x10000000

#define IER 0
#define LCR 3

#define reg(r) (volatile char*) (UART0 + r)
#define WriteReg(r,x) (*(reg(r)) = x)
#define ReadReg(r) (*(reg(r)))


void uartinit()
{
  // выключить прерывания
  WriteReg(IER, 0x00);
  // включить 8-битный режим
  WriteReg(LCR, 0x03);
}

void uartputc(char c)
{
    WriteReg(0, c);
}
