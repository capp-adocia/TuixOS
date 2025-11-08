/* kernel.ld */
ENTRY(_start)

SECTIONS
{
    _kernel_start = 0x10000;          /* 内核加载地址 */
    . = _kernel_start;
    
    .multiboot_header : {
        *(.multiboot_header)
    }

    .text : {
        _kernel_text_start = .;
        *(.text)
        *(.text.*)
        _kernel_text_end = .;
    }
    
    .rodata : {
        _kernel_rodata_start = .;
        *(.rodata)
        *(.rodata.*)
        _kernel_rodata_end = .;
    }
    
    .data : {
        _kernel_data_start = .;
        *(.data)
        *(.data.*)
        _kernel_data_end = .;
    }
    
    .bss : {
        _kernel_bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        _kernel_bss_end = .;
    }

    _kernel_end = .; /* 内核结束地址 */
}