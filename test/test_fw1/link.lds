/* link.lds */
OUTPUT_ARCH("riscv")

MEMORY
{
    ROM (rx): ORIGIN = 0x80000000, LENGTH = 128K
    RAM (rw): ORIGIN = 0x00000000, LENGTH = 64K
}
ENTRY(_start)
_system_stack_size = 0x800;

SECTIONS
{
    .text :
    {
        . = ALIGN(4);
        PROVIDE(_stext = .);
        *(.text.entry)
        *(.text)
        *(.rodata)
        . = ALIGN(4);
        PROVIDE(_etext = .);
    } > ROM = 0

    . = ALIGN(4);
    _ldata = LOADADDR(.data);
    .data :
    {
        . = ALIGN(4);
        PROVIDE(_sdata = .);
        *(.data)
        . = ALIGN(4);
        PROVIDE(_edata = .);
    } > RAM

    .stack :
    {
        . = ALIGN(4);
        PROVIDE(_sstack = .);
        . = . + _system_stack_size;
        . = ALIGN(4);
        PROVIDE(_estack = .);
    } > RAM

    .bss :
    {
        . = ALIGN(4);
        PROVIDE(_sbss = .);
        _sbss = .;
        *(.bss)
        *(.bss.*)
        . = ALIGN(4);
        PROVIDE(_ebss = .);
    } > RAM
}
