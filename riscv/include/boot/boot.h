#ifndef BOOT_H
#define BOOT_H

extern unsigned long _sp;
extern char _end[];

void boot_console_init();

#endif /* BOOT_H */
