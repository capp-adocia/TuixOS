; kernel/asm/switch.asm

section .text
global switch_to
switch_to:
    mov eax, [esp + 4]    ; eax = prev
    mov edx, [esp + 8]    ; edx = next

    ; 保存上一个进程的上下文
    push ebp
    push ebx
    push esi
    push edi

    ; 切换栈指针
    ; prev->ctx = 寄存器esp
    ; 寄存器esp = next->ctx
    mov [eax], esp        ; 保存当前ESP到prev->ctx中
    mov esp, [edx]        ; 更新当前的ESP

    ; 恢复下一个进程的上下文
    pop edi
    pop esi
    pop ebx
    pop ebp

    ret                   ; 返回到next进程的eip
