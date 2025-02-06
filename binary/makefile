PREFIX = x86_64-linux-gnu-
CP = ${PREFIX}g++
CC = ${PREFIX}gcc
LD = ${PREFIX}ld
OBJDUMP = ${PREFIX}objdump
OBJCOPY = ${PREFIX}objcopy

.PHONY: clean

CCFLAGS = -Wall -g

main.o: main.c
	${CC} ${CCFLAGS} -c $< -o $@

img: img.c
	${CC} ${CCFLAGS} $< -c -o img
	# ${OBJCOPY} -S -O binary img.o img.bin
	${LD} -r -b binary -o img.bin img

out: main img
	${LD} -o out.elf main.o -lm -lc -e main -b binary img.bin
	${OBJDUMP} -t out.elf > out.sym

clean:
	rm -f _* *.o *.elf *.sym *.bin