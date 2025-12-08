/* include/Tuix/ptrace.h */

#ifndef I_T_PTRACE_H
#define I_T_PTRACE_H

#include <stddef.h>

struct pt_regs
{
    uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t int_no, err_code;                          // 2.中断号和错误代码
    uint32_t eip, cs, eflags, user_esp, ss;             // 1.CPU自动压栈
} __attribute__((packed));

#endif
