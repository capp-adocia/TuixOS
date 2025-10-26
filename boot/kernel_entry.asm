; boot/kernel_entry.asm 使用GRUB加载

section .text
global _start
extern kernel_main

section .multiboot_header
header_start:
    dd 0xe85250d6                ; Multiboot2 魔数
    dd 0                         ; 架构 0 (保护模式 i386)
    dd header_end - header_start ; 头长度
    ; 校验和
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))
    ; 结束标签
    dw 0    ; 类型: 结束 (0)
    dw 0    ; 标志  
    dd 8    ; 大小: 8 字节
header_end:

; 内核真正入口
section .text
_start:
    mov esp, kernel_stack_top
    push ebx ; multiboot_info 结构指针
    push eax ; magic number
    call kernel_main
    
    cli
.hang:
    hlt
    jmp .hang

; 内核栈空间
align 16
global kernel_stack_top
kernel_stack_bottom:
    resb 16384 ; 16KB
kernel_stack_top: