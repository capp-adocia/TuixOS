/* kernel/trap/idt.c */

#include <Tuix/trap_frame.h>
#include <Tuix/idt.h>
#include <Tuix/idt_list.h>
#include <Tuix/syscall.h>

struct idt_entry idt[IDT_ITEM_NUM];
interrupt_handler_t interrupt_handlers[IDT_ITEM_NUM];
struct idt_ptr idtp;
static inline void idt_load(uint32_t idt_ptr) {__asm__ volatile("lidt (%0)" : : "r"(idt_ptr));}
static void idt_set(void);
static void register_interrupt_handlers(void);

void init_idt(void)
{
    idtp.limit = sizeof(struct idt_entry) * IDT_ITEM_NUM - 1;
    idtp.base = (uint32_t)&idt;
    // 设置桩函数
    idt_set();
    // 设置C中断处理函数
    register_interrupt_handlers();
    // 加载IDT
    idt_load((uint32_t)&idtp);
}

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt[num].base_low = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

void isr_handler(struct trap_frame* frame)
{
    if(interrupt_handlers[frame->int_no])
        interrupt_handlers[frame->int_no](frame);
    else isr_default_handler(frame); // 默认处理
}

static void register_interrupt_handlers(void)
{
    /* 注册第32-47的中断处理函数和第128号系统调用处理函数 */
#define X(num, name) register_interrupt_handler(num, isr_##name##_handler);
    IDT_LIST_EXP
    IDT_LIST_PIC
    IDT_LIST_SYSCALL
#undef X
    // 先不设置设置其余48-255
// #define X(num) register_interrupt_handler(num, isr_##num##_handler);
    // IDT_LIST_OTHER
// #undef X
}

static void idt_set(void)
{
    // 全部设置好
#define X(num, name) idt_set_gate(num, (uint32_t)isr_##name##_stub, 0x08, 0x8E);
    IDT_LIST_EXP
    IDT_LIST_PIC
#undef X
    // 系统调用这里要注意dpl设置为3，否则会触发通用寄存器错
#define X(num, name) idt_set_gate(num, (uint32_t)isr_##name##_stub, 0x08, 0xEE);
    IDT_LIST_SYSCALL
#undef X
// 设置num暂时没name
#define X(num) idt_set_gate(num, (uint32_t)isr_##num##_stub, 0x08, 0x8E);
    IDT_LIST_OTHER
#undef X
}
