/* include/Silan/interrupts.h 中断处理函数（软件层面） */

#ifndef I_H_INTERRUPTS_H
#define I_H_INTERRUPTS_H

#include <stddef.h>
#include <Silan/idt_list.h>

struct interrupt_frame
{
    uint32_t es;
    uint32_t ds;
    uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t int_no, err_code;                          // 2.中断号和错误代码
    uint32_t eip, cs, eflags, user_esp, ss;             // 1.CPU自动压栈
} __attribute__((packed));

// 每个中断号对应的处理函数 interrupt_frame
typedef void (*interrupt_handler_t)(struct interrupt_frame*);

/**
 * 中断默认处理
 * @param frame 栈帧
 */
void isr_default(struct interrupt_frame* frame);

// 这里只定义了前32个
#define X(num) void isr_##num##_c(struct interrupt_frame* frame);
IDT_LIST_EXP
#undef X

// 33
void isr_keyboard(struct interrupt_frame* frame);
#endif