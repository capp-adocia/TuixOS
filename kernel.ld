/* kernel.ld */
ENTRY(_start)

SECTIONS
{
    . = 0x10000;          /* 内核加载地址 */
    
    .multiboot_header : {
        *(.multiboot_header)
    }

    .text : {
        *(.text)
        *(.text.*)
    }
    
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }
    
    .data : {
        *(.data)
        *(.data.*)
    }
    
    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }
}