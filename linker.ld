ENTRY(_start)
SECTIONS
{
    . = 0x9000;
    .text : { *(.text) *(.text.*) }
    .rodata : { *(.rodata) *(.rodata.*) }
    .data : { *(.data) *(.data.*) }
    .bss : { *(.bss) *(.bss.*) *(COMMON) }
}
