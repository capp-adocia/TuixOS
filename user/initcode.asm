; kernel/user/initcode.asm

[BITS 32]

global user_init_code
global user_init_code_size

section .text
user_init_code:
.loop:
    mov eax, 16
    mov ebx, 1        ; fd = 1 (stdout)
    mov ecx, message  ; buf = 字符串地址
    mov edx, msg_len  ; count = 字符串长度
    int 0x80

    mov eax, 2
    int 0x80

    jmp $

message:
    db "Hello, 用户进程!", 0xA  ; 0xA = 换行
msg_len equ $ - message

user_init_code_size:
    dd $ - user_init_code  ; 计算大小
