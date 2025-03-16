#ifndef UART_H

#include "kernel/types.h"
#include "kernel/bitops.h"

#define CLK_FREQ        (25175000)
#define UART_BAUDRATE   (115200)

#define UART_BASE       (0x83000000ul)

#define UART_RX         (REG32(UART_BASE + 0))
#define UART_TX         (REG32(UART_BASE + 0))
#define UART_DIV        (REG32(UART_BASE + 4))


void uart_init();

#endif /* UART_H */