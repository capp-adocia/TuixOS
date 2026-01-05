; kernel/user/initcode.asm

[BITS 32]

global user_init_code
global user_init_code_size

section .text
user_init_code:
.loop:
    mov eax, 2
    int 0x80
    jmp .loop

user_init_code_size:
    dd $ - user_init_code  ; 计算大小

