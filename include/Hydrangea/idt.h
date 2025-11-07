/* include/Hydrangea/idt.h */

#ifndef I_H_IDT_H
#define I_H_IDT_H

#include <stddef.h>
#include <Hydrangea/isr.h>
#include <Hydrangea/interrupts.h>

/* 中断数量 */
#define IDT_ITEM_NUM 256

/* IDT条目 */
struct idt_entry {
    uint16_t base_low;    // 处理函数地址低16位
    uint16_t selector;    // 代码段选择子
    uint8_t zero;         // 必须为0
    uint8_t flags;        // 类型属性
    uint16_t base_high;   // 处理函数地址高16位
} __attribute__((packed));

/* IDT指针 */
struct idt_ptr {
    uint16_t limit;       // IDT表大小-1
    uint32_t base;        // IDT表基地址
} __attribute__((packed));

// 全局IDT表
extern struct idt_entry idt[IDT_ITEM_NUM];
// 中断处理函数表
extern interrupt_handler_t interrupt_handlers[IDT_ITEM_NUM];

/**
 * 初始化IDT
 */
void init_idt(void);

/*
中断总流程：
（硬件层面自动完成）
    当中断发生时，cpu会去查IDT表，根据idt_set_gate的设置（idt_init）
    跳转至对于汇编桩函数，然后统一再跳转到(isr_common)根据编号查表
（软件层面接力）
    找到对应的软件处理函数，调用相应的中断处理函数
    处理完成后返回(isr_common)，恢复寄存器，iret返回
*/

/**
 * 设置IDT条目
 * @param num 数量
 * @param base 基址
 * @param sel 代码段选择子
 * @param flags 类型属性
 */
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

/**
 * 注册中断函数
 * @param n 编号
 * @param handler 待注册函数的指针
 */
inline void register_interrupt_handler(uint8_t n, interrupt_handler_t handler)
{
    interrupt_handlers[n] = handler;
}

/**
 * 统一让isr跳转到这里查表
 */
void isr_common(void);

/**
 * 分发函数
 * @param frame 栈帧指针
 */
void isr_handler(struct interrupt_frame* frame);

#endif