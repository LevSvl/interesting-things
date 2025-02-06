PREFIX = x86_64-linux-gnu-
CP = ${PREFIX}g++
CC = ${PREFIX}gcc
LD = ${PREFIX}ld

.PHONY: clean

CCFLAGS = -Wall -g

inl: inl1.cpp inl2.cpp
	${CP} ${CCFLAGS} -c inl1.cpp -o inl1.o
	${CP} ${CCFLAGS} -c inl2.cpp -o inl2.o
	${CP} ${CCFLAGS} inl1.o inl2.o -o _inl
%: %.c
	${CC} ${CCFLAGS} $< -o _$@

%: %.cpp
	${CP} ${CCFLAGS} $< -o _$@

clean:
	rm -f _*