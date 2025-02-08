PREFIX = x86_64-linux-gnu-
CP = ${PREFIX}g++
CC = ${PREFIX}gcc
LD = ${PREFIX}ld

.PHONY: clean

CCFLAGS = -g -O0

SRC = $(wildcard *.c)

main: ${SRC}
	${CC} ${CCFLAGS} ${SRC} -o _main

clean:
	rm -f _*