#include "kernel/devices/uart.h"

void uart_init()
{
    UART_DIV = CLK_FREQ / UART_BAUDRATE - 2;
}