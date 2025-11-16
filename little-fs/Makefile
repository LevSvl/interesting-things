PREFIX = x86_64-linux-gnu-
CP = ${PREFIX}g++
CC = ${PREFIX}gcc
LD = ${PREFIX}ld

.PHONY: clean run

CCFLAGS = -Wall -g

all: little_fs main

%: %.c
	${CC} ${CCFLAGS} $< -o _$@ -Wl,-Map=main.map

fs: little_fs
	./_little_fs Makefile

run: fs main 
	./_main

clean:
	rm -f _* *.img