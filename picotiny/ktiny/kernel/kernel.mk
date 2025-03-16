include ${ROOT_DIR}/Common.mk

LDSCRIPT ?= kernel.ld

MABI = ilp32
MARCH= rv32im

CFLAGS += -fstrict-volatile-bitfields -Wl,-Bstatic -Wl,--gc-sections -ffunction-sections -fno-pie
CFLAGS += -march=$(MARCH)  -mabi=$(MABI) -ffreestanding -nostartfiles -mcmodel=medany
LDFLAGS +=  -nostdlib -T$(LDSCRIPT) -Map $(BUILD_DIR)/out.map --print-memory-usage -m elf32lriscv_ilp32

SRC += $(wildcard *.S *.c *.cpp)
OBJS += $(patsubst %,${BUILD_DIR}/%, 								\
			$(patsubst %.S,%.o,										\
				$(patsubst %.c,%.o,									\
					$(patsubst %.cpp,%.o,${SRC})))					)

SUB_MAKEFILES =  ${shell grep '^IM_MODULE = true' -R * -l }
SUB_DIRS = ${shell dirname ${SUB_MAKEFILES}}

SUB_SRC = ${shell find ${SUB_DIRS} -name "*.[c,S,cpp]"}
SUB_OBJS = $(patsubst %,${BUILD_DIR}/%, 							\
			$(patsubst %.S,%.o,										\
				$(patsubst %.c,%.o,									\
					$(patsubst %.cpp,%.o,							\
						${SUB_SRC})))	)							\

include ${SUB_MAKEFILES}

all: sub_dirs ${SUB_OBJS} ${OBJS} 									\
	$(BUILD_DIR)/out.elf 											\
	$(BUILD_DIR)/out.hex 											\
	$(BUILD_DIR)/out.asm 											\
	$(BUILD_DIR)/out.v 												\
	$(BUILD_DIR)/out.sym

.PHONY: program clean

program:
	python3 ${PROGRAMMER} ${BUILD_DIR}/out.v $(COMx)


			
