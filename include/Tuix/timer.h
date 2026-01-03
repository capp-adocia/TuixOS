/* include/Tuix/timer.h */

#ifndef I_T_TIMER_H
#define I_T_TIMER_H

#include <stddef.h>
#include <Tuix/trap_frame.h>

extern volatile uint32_t timer_ticks;

/**
 * 初始化定时器
 * @param frequency 频率 单位：HZ
 */
void init_timer(uint32_t frequency);

/**
 * 处理定时中断
 * @param regs 保存的中断帧数据
 */
void timer_handler(struct trap_frame* regs);

#endif
