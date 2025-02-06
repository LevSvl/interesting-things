PREFIX = x86_64-linux-gnu-

CC = ${PREFIX}gcc
LD = ${PREFIX}ld

.PHONY: clean

OBJECTS = main.o ipdata.o ipfuncs.o 

CCFLAGS = -Wall -g

task: main
	./_main

%: %.c
	${CC} ${CCFLAGS} $< -o _$@

main: ${OBJECTS}
	${CC} ${CCFLAGS} ${OBJECTS} -o _$@

clean:
	rm -f _*
	rm -f *.o