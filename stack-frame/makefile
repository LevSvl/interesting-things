PREFIX = riscv64-linux-gnu-
CP = ${PREFIX}g++
CC = ${PREFIX}gcc
LD = ${PREFIX}ld

.PHONY: clean

CCFLAGS = -Wall -g -nostartfiles

%: %.c
	${CC} ${CCFLAGS} $< -o _$@
	${PREFIX}objdump -S _$@ > _$@.asm

clean:
	rm -f _*