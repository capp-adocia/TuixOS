/* include/Silan/pic.h - 8259A PIC操作 */

#ifndef I_PIC_H
#define I_PIC_H

#include <stddef.h>

// IRQ0   - 系统定时器      (最高优先级)
// IRQ1   - 键盘
// IRQ2   - 从PIC的级联线
// IRQ3   - COM2串口
// IRQ4   - COM1串口  
// IRQ5   - LPT2打印机
// IRQ6   - 软盘驱动器
// IRQ7   - LPT1打印机

// IRQ8   - 实时时钟
// IRQ9   - 可用
// IRQ10  - 可用
// IRQ11  - 可用
// IRQ12  - 鼠标
// IRQ13  - 数学协处理器
// IRQ14  - 主硬盘
// IRQ15  - 从硬盘        (最低优先级)
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