/* include/Hydrangea/interrupts.h 中断处理函数（软件层面） */

#ifndef I_H_INTERRUPTS_SCREEN_H
#define I_H_INTERRUPTS_SCREEN_H

#include <stddef.h>

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

// 0: 除零错误
void isr_0_c(struct interrupt_frame* frame);

// 1: 调试异常
void isr_1_c(struct interrupt_frame* frame);

// 2: 非屏蔽中断
void isr_2_c(struct interrupt_frame* frame);
// 3: 断点
void isr_3_c(struct interrupt_frame* frame);

// 4: 溢出
void isr_4_c(struct interrupt_frame* frame);

// 5: 边界检查
void isr_5_c(struct interrupt_frame* frame);

// 6: 无效操作码
void isr_6_c(struct interrupt_frame* frame);

// 7: 设备不可用
void isr_7_c(struct interrupt_frame* frame);

// 8: 双重故障
void isr_8_c(struct interrupt_frame* frame);

// 9: 协处理器段越界
void isr_9_c(struct interrupt_frame* frame);

// 10: 无效TSS
void isr_10_c(struct interrupt_frame* frame);

// 11: 段不存在
void isr_11_c(struct interrupt_frame* frame);

// 12: 栈段错误
void isr_12_c(struct interrupt_frame* frame);

// 13: 通用保护错误
void isr_13_c(struct interrupt_frame* frame);

// 14: 页错误
void isr_14_c(struct interrupt_frame* frame);

// 15: 保留
void isr_15_c(struct interrupt_frame* frame);

// 16: 浮点错误
void isr_16_c(struct interrupt_frame* frame);

// 17: 对齐检查
void isr_17_c(struct interrupt_frame* frame);

// 18: 机器检查
void isr_18_c(struct interrupt_frame* frame);

// 19-31: 保留和特定平台异常
void isr_19_c(struct interrupt_frame* frame);
void isr_20_c(struct interrupt_frame* frame);
void isr_21_c(struct interrupt_frame* frame);
void isr_22_c(struct interrupt_frame* frame);
void isr_23_c(struct interrupt_frame* frame);
void isr_24_c(struct interrupt_frame* frame);
void isr_25_c(struct interrupt_frame* frame);
void isr_26_c(struct interrupt_frame* frame);
void isr_27_c(struct interrupt_frame* frame);
void isr_28_c(struct interrupt_frame* frame);
void isr_29_c(struct interrupt_frame* frame);
void isr_30_c(struct interrupt_frame* frame);
void isr_31_c(struct interrupt_frame* frame);

#endif