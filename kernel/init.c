/* kernel/user/initcode.c */

#include <stddef.h>

const uint8_t user_init_code[] = {
    // 写入代码页末尾（0xF00 开始）
    0xb8, 0x00, 0x00, 0x00, 0x00,        // mov eax, 0
    
    // write_loop:
    0xa3, 0x00, 0x01, 0x00, 0x00,        // mov [0x0100], eax
    0x40,                                // inc eax
    0xeb, 0xf9                           // jmp $-7
};

const uint32_t user_init_code_size = sizeof(user_init_code);
