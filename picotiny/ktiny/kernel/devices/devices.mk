include ${ROOT_DIR}/Common.mk

IM_MODULE = true

SRC = $(wildcard .S *.c *.cpp)
DEVICES_OBJ = $(patsubst %,${BUILD_DIR}/%,					\
				$(patsubst %.S,%.o,							\
					$(patsubst %.c,%.o,						\
						$(patsubst %.cpp,%.o,${SRC}))))


all: ${DEVICES_OBJ}