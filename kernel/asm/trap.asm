; kernel/asm/trap.asm

section .text
global trap
trap:
    push ds
    push es
    push fs
    push gs
    pusha                     ; 压入 eax,ecx,edx,ebx,esp,ebp,esi,edi

    mov ax, 0x10              ; 内核数据段选择子
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp                  ; 压入当前栈指针（指向陷阱帧）
    extern isr_handler        ; 声明外部函数
    call isr_handler
    add esp, 4

global trap_ret
trap_ret:
    popa
    pop gs
    pop fs
    pop es
    pop ds

    add esp, 8                ; 跳过错误代码和中断号
    iret
