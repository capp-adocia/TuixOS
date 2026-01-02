/* kernel.ld */
ENTRY(_start) /* 内核程序开始执行的入口地址 */

SECTIONS
{
    _kernel_start = 0x80100000;     /* 内核加载到的虚拟地址 */
    . = _kernel_start;
    
    .multiboot_header : AT(0x100000) {
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
    . = ALIGN(0x1000);
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
