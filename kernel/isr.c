/* kernel/isr.c */

#include <Hydrangea/isr.h>

#define ISR_NOERRCODE(num) \
void isr_##num(void) { \
    __asm__ volatile("pushl $0\n\t" "pushl $" #num "\n\t" "jmp isr_common"); \
}

#define X(num) ISR_NOERRCODE(num)
    IDT_LIST_EXP // 0-31: CPU 异常
    IDT_LIST_PIC // 32-47: 硬件中断 (IRQ0-IRQ15)
    IDT_LIST_OTHER // 48-255: 保留和用户定义中断
#undef X