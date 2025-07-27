# Сборка
PREFIX = riscv64-linux-gnu-
CC = $(PREFIX)gcc
AS = $(PREFIX)as
LD = $(PREFIX)ld
OBJDUMP = $(PREFIX)objdump
OBJCOPY = $(PREFIX)objcopy

ENTRY_POINT = 0x40000000
ENTRY_SYMBOL = main

CCFLAGS = -g -Wall -I$(INCLUDE_DIR) -ffreestanding
LDFLAGS = -Ttext $(ENTRY_POINT) -e $(ENTRY_SYMBOL) -T kernel.ld

TARGET_DIR := build
INCLUDE_DIR = include

SOURCES = $(shell find -name "*.c" | cut -d '/' -f 2-)
SOURCES_ASM = $(shell find -name "*.S" | cut -d '/' -f 2-)

OBJECTS = $(patsubst %.c,$(TARGET_DIR)/%.o,$(SOURCES))
OBJECTS_ASM = $(patsubst %.S,$(TARGET_DIR)/%.o,$(SOURCES_ASM))

SUB_DIRS = $(shell dirname $(OBJECTS))

KERNEL_ELF = $(TARGET_DIR)/kernel.elf
KERNEL_SYM = $(TARGET_DIR)/kernel.sym
KERNEL_ASM = $(TARGET_DIR)/kernel.asm

DTS = $(shell find -name "*.dts" | cut -d '/' -f 2-)
DTB = $(patsubst %.dts,$(TARGET_DIR)/%.dtb,$(DTS))
DTB_O = $(patsubst %.dtb,%.dtb.o,$(DTB))
OBJECTS += $(DTB_O)

# Создание результирующей директории сборки
$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

# Создание директории подмодуля
$(TARGET_DIR)/%:
	mkdir -p $@

# Сборка объектного файла
$(TARGET_DIR)/%.o: %.S
	${AS} $< -o $@

$(TARGET_DIR)/%.o: %.c
	${CC} ${CCFLAGS} -c $< -o $@

# Сборка dtb
$(TARGET_DIR)/%.dtb: %.dts
	dtc -I dts -O dtb -o $@ $<

# Генерация объектного файла dtb
$(TARGET_DIR)/%.dtb.o: $(TARGET_DIR)/%.dtb
	$(OBJCOPY) -I binary -O elf64-littleriscv $< $@

# Сборка итогового ELF-файла
$(KERNEL_ELF): $(SUB_DIRS) $(DTB) $(OBJECTS) $(OBJECTS_ASM)
	echo $(OBJECTS_ASM)
	${LD} ${LDFLAGS} $(OBJECTS) $(OBJECTS_ASM) -o $@

$(KERNEL_ASM): $(KERNEL_ELF)
	$(OBJDUMP) -d $< > $@

# Генерация таблицы символов
$(KERNEL_SYM): $(KERNEL_ELF)
	$(OBJDUMP) -t $< > $@



# Эмуляция
QEMU = qemu-system-riscv64
MACHINE = virt
QEMUOPTS = --nographic --kernel $(KERNEL_ELF) -s -S

run_qemu: all
	$(QEMU) -M $(MACHINE) $(QEMUOPTS)



# Общие правила
all: $(TARGET_DIR) $(KERNEL_ELF) $(KERNEL_SYM) $(KERNEL_ASM)

clean:
	rm -rf $(TARGET_DIR) $(CONFIG)
