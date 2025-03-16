LDSCRIPT ?= linker_flash.ld

MABI = ilp32
MARCH= rv32im



CFLAGS += -fstrict-volatile-bitfields -Wl,-Bstatic -Wl,--gc-sections -ffunction-sections -fno-pie
CFLAGS += -march=$(MARCH)  -mabi=$(MABI) -ffreestanding -nostartfiles -mcmodel=medany
LDFLAGS +=  -nostdlib -T$(LDSCRIPT) -Map $(BUILD_DIR)/out.map --print-memory-usage -m elf32lriscv_ilp32

SRC = $(wildcard *.S *.c *.cpp)

OBJS = $(patsubst %,${BUILD_DIR}/%, 								\
			$(patsubst %.S,%.o,										\
				$(patsubst %.c,%.o,									\
					$(patsubst %.cpp,%.o,${SRC})))					)

SUB_MAKEFILES = ${shell grep -l IM_MODULE -R -f ${ROOT_DIR}/kernel}

SUB_OBJS = $(patsubst %,${BUILD_DIR}/%, 							\
			$(patsubst %.S,%.o,										\
				$(patsubst %.c,%.o,									\
					$(patsubst %.cpp,%.o,							\
						${${shell find ${ROOT_DIR}/kernel -name "*.[c,S,cpp]"}})))	)

export BUILD_DIR

sub_dirs:


%.mk:
	${MAKE} -C ${shell dirname %} -f ${shell basename $@} all

all: ${OBJS} sub_dirs
	$(BUILD_DIR)/out.elf 	\
	$(BUILD_DIR)/out.hex 	\
	$(BUILD_DIR)/out.asm 	\
	$(BUILD_DIR)/out.v 		\
	$(BUILD_DIR)/out.sym


include ${ROOT_DIR}/Common.mk

.PHONY: program clean

program:
	python3 ${PROGRAMMER} ${BUILD_DIR}/out.v $(COMx)


			
