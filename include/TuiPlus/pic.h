/* include/TuiPlus/pic.h - 8259A PIC操作 */

#ifndef I_PIC_H
#define I_PIC_H

#include <stddef.h>

/* PIC IRQ 编号 */
#define IRQ_TIMER       0   // 系统定时器      (最高优先级)
#define IRQ_KEYBOARD    1   // 键盘
#define IRQ_CASCADE     2   // 从PIC的级联线
#define IRQ_COM2        3   // COM2串口
#define IRQ_COM1        4   // COM1串口
#define IRQ_LPT2        5   // LPT2打印机
#define IRQ_FLOPPY      6   // 软盘驱动器
#define IRQ_LPT1        7   // LPT1打印机
#define IRQ_RTC         8   // 实时时钟
#define IRQ_PS2_MOUSE   12  // 鼠标
#define IRQ_FPU         13  // 数学协处理器
#define IRQ_ATA1        14  // 主硬盘
#define IRQ_ATA2        15  // 从硬盘         (最低优先级)

#define PIC1_CMD    0x20    // 主PIC命令端口
#define PIC1_DATA   0x21    // 主PIC数据端口
#define PIC2_CMD    0xA0    // 从PIC命令端口  
#define PIC2_DATA   0xA1    // 从PIC数据端口

#define ICW1_ICW4   0x01    // 需要ICW4
#define ICW1_INIT   0x10    // 初始化命令

/**
 * 初始化pic,默认禁用所有中断
 */
void init_pic(void);

/**
 * 屏蔽PIC中断
 */
void disable_pic(void);

/**
 * 启用特定IRQ
 * @param irq
 */
void enable_irq(uint8_t irq);

/**
 * 禁用特定IRQ
 * @param irq
 */
void disable_irq(uint8_t irq);

/**
 * 发送EOI中断结束信号
 * @param irq
 */
void send_eoi(uint8_t irq);

#endif