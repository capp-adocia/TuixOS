/* driver/pic.c - 8259A PIC初始化 */

#include <Hydrangea/io.h>
#include <Hydrangea/pic.h>

#define PIC1_CMD    0x20    // 主PIC命令端口
#define PIC1_DATA   0x21    // 主PIC数据端口
#define PIC2_CMD    0xA0    // 从PIC命令端口  
#define PIC2_DATA   0xA1    // 从PIC数据端口

#define ICW1_ICW4   0x01    // 需要ICW4
#define ICW1_INIT   0x10    // 初始化命令

/* 初始化8259A PIC */
void init_pic(void)
{
    // 保存当前掩码
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);
    
    // 初始化主PIC
    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);  // ICW1
    outb(PIC1_DATA, 0x20);                   // ICW2: 中断向量偏移0x20
    outb(PIC1_DATA, 0x04);                   // ICW3: IRQ2连接从PIC
    outb(PIC1_DATA, 0x01);                   // ICW4: 8086模式
    
    // 初始化从PIC
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);  // ICW1
    outb(PIC2_DATA, 0x28);                   // ICW2: 中断向量偏移0x28
    outb(PIC2_DATA, 0x02);                   // ICW3: 连接到主PICIRQ2
    outb(PIC2_DATA, 0x01);                   // ICW4: 8086模式
    
    // 恢复掩码（屏蔽所有中断）
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

/* 屏蔽PIC中断 */
void disable_pic(void)
{
    outb(PIC1_DATA, 0xFF);  // 屏蔽主PIC所有中断
    outb(PIC2_DATA, 0xFF);  // 屏蔽从PIC所有中断
}

/* 启用特定IRQ */
void enable_irq(uint8_t irq)
{
    uint16_t port;
    uint8_t value;
    
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

/* 禁用特定IRQ */
void disable_irq(uint8_t irq)
{
    uint16_t port;
    uint8_t value;
    
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = inb(port) | (1 << irq);
    outb(port, value);
}

/* 发送EOI（中断结束）信号 */
void send_eoi(uint8_t irq)
{
    if (irq >= 8) {
        outb(PIC2_CMD, 0x20);  // 从PIC EOI
    }
    outb(PIC1_CMD, 0x20);      // 主PIC EOI
}