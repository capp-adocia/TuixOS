/* include/Tuix/trap_frame.h */

#ifndef I_T_TRAPFRAME_H
#define I_T_TRAPFRAME_H

#include <stddef.h>

struct trap_frame
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;   // pusha保存的ESP值
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t int_no, err_code;                          // 2.中断号和错误代码
    uint32_t eip, cs, eflags, user_esp, ss;             // 1.CPU自动压栈
} __attribute__((packed));

#endif
