/* kernel/driver/pic.c - 8259A PIC初始化 */

#include <Tuix/pic.h>
#include <arch/x86.h>

/* 初始化8259A PIC */
void init_pic(void)
{
    // 初始化序列
    outb(PIC1_CMD, 0x11);    // ICW1: 初始化, 需要ICW4
    outb(PIC2_CMD, 0x11);    // ICW1: 初始化, 需要ICW4

    outb(PIC1_DATA, 0x20);   // ICW2: 主PIC中断向量 0x20-0x27
    outb(PIC2_DATA, 0x28);   // ICW2: 从PIC中断向量 0x28-0x2F

    outb(PIC1_DATA, 0x04);   // ICW3: 主PIC - IRQ2上有从PIC
    outb(PIC2_DATA, 0x02);   // ICW3: 从PIC - 级联到IRQ2

    outb(PIC1_DATA, 0x01);   // ICW4: 8086模式
    outb(PIC2_DATA, 0x01);   // ICW4: 8086模式

    disable_pic();
}

void disable_pic(void)
{
    outb(PIC1_DATA, 0xFF);  // 屏蔽主PIC所有中断
    outb(PIC2_DATA, 0xFF);  // 屏蔽从PIC所有中断
}

void enable_irq(uint8_t irq)
{
    uint16_t port;
    uint8_t value;

    if (irq < 8)
        port = PIC1_DATA;

    else
    {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

void disable_irq(uint8_t irq)
{
    uint16_t port;
    uint8_t value;

    if (irq < 8)
        port = PIC1_DATA;

    else
    {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = inb(port) | (1 << irq);
    outb(port, value);
}

void send_eoi(uint8_t irq)
{
    if (irq >= 8)
        outb(PIC2_CMD, 0x20);  // 从PIC EOI

    outb(PIC1_CMD, 0x20);      // 主PIC EOI
}
