/* include/Tuix/isr.h 中断入口桩（硬件层面） */

#ifndef I_T_ISR_H
#define I_T_ISR_H

#include <Tuix/idt_list.h>

#define X(num, name) void isr_##name##_stub(void);
    IDT_LIST_EXP // 0-31: CPU 异常
    IDT_LIST_PIC // 32-47: 硬件中断 (IRQ0-IRQ15)
    IDT_LIST_SYSCALL // 128: 系统调用
#undef X

#define X(num) void isr_##num##_stub(void);
    IDT_LIST_OTHER // 48-255: 保留和用户定义中断
#undef X

#endif
