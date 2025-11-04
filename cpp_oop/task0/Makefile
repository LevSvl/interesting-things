PREFIX = # riscv64-linux-gnu
CPP = ${PREFIX}g++

.PHONY: clean

CCFLAGS = -Wall -g

%.o: %.cpp
	${CPP} ${CCFLAGS} -c $< -o $@

CPP_OBJECTS = Characteristic.o CharacteristicTable.o main.o

a.out: $(CPP_OBJECTS)
	$(CPP) $(CPP_OBJECTS) -o a.out

run: a.out
	@./a.out


clean:
	rm -f *.o *.out