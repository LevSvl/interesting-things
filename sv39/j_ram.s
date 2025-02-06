.text

.globl main

go_ram:
  lui sp, 0xFFFF
  j main
  nop