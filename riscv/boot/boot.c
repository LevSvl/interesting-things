#include "types.h"
#include "boot/boot.h"
#include "device_tree/fdt.h"

#include "drivers/console.h"

unsigned long _sp;

void boot_init_stack_pointer()
{
    _sp = ((((unsigned long)_end)+4096-1) & ~(4096-1));
}

extern uintptr_t uart_base;

void boot_console_init()
{
    printf("boot\n");
}
