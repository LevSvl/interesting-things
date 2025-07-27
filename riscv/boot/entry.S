.section .text
.global _start
.global _sp

_start:
    jal install_stack_pointer
    j main

# set up a stack for C
install_stack_pointer:
    jal boot_init_stack_pointer
    la sp, _sp
    li a0, 1024*4
    csrr a1, mhartid
    addi a1, a1, 1
    mul a0, a0, a1
    add sp, sp, a0
