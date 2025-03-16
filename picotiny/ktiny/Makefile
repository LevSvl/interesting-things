# Flash parameters
PROGRAMMER = ${PWD}/flash/sw/pico-programmer.py
COMx ?= /dev/ttyS10

export PROGRAMMER COMx 


# Build parameters
PREFIX ?= riscv64-linux-gnu-

CC = $(PREFIX)gcc
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy
OBJDUMP = $(PREFIX)objdump

export CC LD OBJCOPY OBJDUMP

ROOT_DIR = ${shell pwd}
BUILD_DIR = ${PWD}/build
INCLUDE += -I${PWD}/include

KERNEL_MAKEFILE ?= kernel.mk
USER_MAKEFILE ?= none

export ROOT_DIR BUILD_DIR INCLUDE

# Rules
all: $(BUILD_DIR) kernel user

$(BUILD_DIR):
	mkdir -p $@

kernel: $(BUILD_DIR)
	${MAKE} -C kernel -f ${KERNEL_MAKEFILE} all

user: $(BUILD_DIR)

program: kernel
	${MAKE} -C kernel -f ${KERNEL_MAKEFILE} program

clean:
	${MAKE} -C kernel -f ${KERNEL_MAKEFILE} clean

.PHONY: kernel user clean


			
