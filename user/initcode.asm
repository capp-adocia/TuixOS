; kernel/user/initcode.asm

global user_init_code
global user_init_code_size

section .text
user_init_code:
.loop:
    ; 测试open系统调用
    mov eax, 15
    mov ebx, path
    mov ecx, 0x200
    int 0x80
    ; 把返回的fd保存起来
    mov ebx, eax ; fd
    mov eax, 16
    mov ecx, path ; buf
    mov edx, msg_len  ; count
    int 0x80

    ; mov eax, 2
    ; int 0x80

    jmp $

message:
    db 0xA, "Hello, 用户进程!", 0xA
msg_len equ $ - message

path:
    db "/hello.txt", 0

user_init_code_size:
    dd $ - user_init_code  ; 计算大小
