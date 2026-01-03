/* kernel/isr.c */

#include <Tuix/isr.h>

#define ISR_NOERRNAME(num, name) \
void isr_##name##_stub(void) { \
    __asm__ volatile("pushl $0\n" "pushl $" #num "\n" "jmp trap"); \
}

#define ISR_NOERRCODE(num) \
void isr_##num##_stub(void) { \
    __asm__ volatile("pushl $0\n" "pushl $" #num "\n" "jmp trap"); \
}

#define X(num, name) ISR_NOERRNAME(num, name)
    IDT_LIST_EXP // 0-31: CPU 异常
    IDT_LIST_PIC // 32-47: 硬件中断 (IRQ0-IRQ15)
#undef X

#define X(num) ISR_NOERRCODE(num)
    IDT_LIST_OTHER // 48-255: 保留和用户定义中断
#undef X
