; kernel/user/init.asm
; 用户的初始化代码

section .text
global user_start
user_start:
    ; 系统调用：write(1, "Hello from user!", 16)
    mov eax, 4      ; sys_write
    mov ebx, 1      ; stdout
    mov ecx, msg
    mov edx, 16
    int 0x80

    ; 系统调用：exit(0)
    mov eax, 1      ; sys_exit
    mov ebx, 0      ; 返回值
    int 0x80

section .data
msg: db "Hello from user!", 0
