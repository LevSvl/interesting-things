PREFIX = riscv64-linux-gnu-
CP = ${PREFIX}g++
CC = ${PREFIX}gcc
AS = ${PREFIX}as
LD = ${PREFIX}ld

.PHONY: clean

CCFLAGS = -Wall -g 

bios: j_ram.s
	${AS} -g $< -o j_ram.elf

%: %.c
	${CC} ${CCFLAGS} $< -o _$@
	${PREFIX}objdump -S _$@ > _$@.asm

qemu: bios main
	qemu-system-riscv64 -kernel _main -bios j_ram.elf -nographic

clean:
	rm -f _*