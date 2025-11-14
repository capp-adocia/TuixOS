/* include/Silan/timer.h */

#ifndef I_S_TIMER_H
#define I_S_TIMER_H

#include <stddef.h>
#include <Silan/interrupts_types.h>

extern volatile uint32_t timer_ticks;

/**
 * 初始化定时器
 * @param frequency 频率 单位：HZ
 */
void init_timer(uint32_t frequency);

/**
 * 处理定时中断
 * @param frame interrupt_frame结构体指针
 */
void timer_handler(struct interrupt_frame* frame);

#endif