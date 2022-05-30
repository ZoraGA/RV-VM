.global _start
.extern main
.section .text
.align 4

_start:
    la sp, _estack
    la a0, _ldata
    la a1, _sdata
    la a2, _edata
    bgeu a1, a2, bss_clear_begin

data_clear:
    lw t0, (a0)
    sw t0, (a1)
    addi a0, a0, 4
    addi a1, a1, 4
    bltu a1, a2, data_clear

bss_clear_begin:
    la a0, _sbss
    la a1, _ebss
    bgeu a0, a1, init_finish

bss_clear:
    sw zero, (a0)
    addi a0, a0, 4
    bltu a0, a1, bss_clear

init_finish:
    call main

__die:
    j __die