; boot/kernel_entry.asm 使用GRUB加载

section .text
global _start
extern kernel_main

%define KERNBASE 0x80000000
%define KERN_ST_PHYS (kernel_stack_top - KERNBASE)
%define ENTRYPGDIR_PHYS (entrypgdir - KERNBASE)

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
    ; 保存一下这些信息以免被破坏
    mov esp, KERN_ST_PHYS
    push ebx ; multiboot_info 结构指针
    push eax ; magic number
    ; 首先先开启分页，设置4MB
    ; 设置双重映射，保证后续代码仍能正常取指令
    mov eax, cr4
    or eax, 0x00000010
    mov cr4, eax
    ; 设置页目录
    mov eax, ENTRYPGDIR_PHYS ; 这里得到的实际是链接器ld设置的虚拟地址
    mov cr3, eax
    ; 开启分页
    mov eax, cr0
    or eax, 0x80010000
    mov cr0, eax
    ; 以下是设置栈指针+调用内核main()
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

; 定义临时页表
section .data
align 4096
entrypgdir:
    ; 设置双重映射，标志位是低12位，而高20位作为页框基地址
    dd 0x00000083 ; 设置标志位都为存在可读写4MB
    times 511 dd 0
    dd 0x00000083
    times 511 dd 0
; 以上等价于xv6中的数组写法
; __attribute__((__aligned__(PGSIZE)))
; pde_t entrypgdir[NPDENTRIES] = {
;   [0] = (0) | PTE_P | PTE_W | PTE_PS,
;   [KERNBASE>>PDXSHIFT] = (0) | PTE_P | PTE_W | PTE_PS,
; };
