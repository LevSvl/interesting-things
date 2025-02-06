PREFIX = x86_64-linux-gnu-
CP = ${PREFIX}g++
CC = ${PREFIX}gcc
LD = ${PREFIX}ld

.PHONY: clean

CCFLAGS = -Wall -g

%: %.c
	${CC} ${CCFLAGS} $< -o _$@

clean:
	rm -f _*