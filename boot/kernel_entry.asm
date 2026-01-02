; boot/kernel_entry.asm 使用GRUB加载

%define KERNBASE 0x80000000
%define ENTRYPGDIR_PHYS (entrypgdir - KERNBASE)

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
global _start
_start:
    ; 保存一下这些信息以免被破坏
    mov esi, eax            ; esi = magic
    mov edi, ebx            ; edi = multiboot_info
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
    ; 在栈上传递参数
    mov esp, boot_stack_top
    push edi
    push esi
    push 0
    ; 以下是设置调用内核main() 小心这里千万不能用call,必须要jmp,因为call是相对跳转,而jmp是按绝对地址跳转
    mov ecx, kernel_main
    jmp ecx
    cli

.hang:
    hlt
    jmp .hang

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

; 内核栈空间放在bss段中，注意不要放错到text段中了
section .bss
align 16

global boot_stack
global boot_stack_top

boot_stack:
    resb 16384
boot_stack_top:
