.globl _start
.globl main


_start:
    lui a0, 0x83000     # 0x83000000 - 0x8300000F UART
    la a1, str

print:
    lbu a2, 0(a1)
    beqz a2, bss_init
    sb a2, 0(a0)
    addi a1, a1, 1 
    j print

crt_start:
    la a0, _bss_start
    la a1, _bss_end
 
bss_init:
    bgeu a0, a1, bss_init_done
    sw zero, 0(a0)
    addi a0, a0, 4
bss_init_done:

    la a0, _sdata
    la a1, _sidata
    la a2, _end

data_init:
    bgeu a1, a2, all_init_done
    
    lw t1, 0(a1)
    sw t1, 0(a0)

    addi a0, a0, 4
    addi a1, a1, 4

all_init_done:
    j main

str:   .string "hello world from ktiny\n\0"

