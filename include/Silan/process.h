/* include/Silan/process.h */

#ifndef I_S_PROCESS_H
#define I_S_PROCESS_H

#include <stddef.h>

struct task_stack
{
    uint32_t* stack_limit;     // 栈顶 低地址
    uint32_t* stack_start;    // 栈底 高地址，栈起始的地址
    uint32_t* current_esp;  // 当前栈指针
};

/**
 * 初始化一个任务
 */
void init_task(void);

/**
 * 创建任务上下文，设置任务栈的一些基本数据
 * @param task 任务栈的结构体指针
 * @param entry_point 任务的入口函数
 */
void setup_task_context(struct task_stack *task, void (*entry_point)());

/**
 * 任务切换
 * @param task_esp 任务栈的栈指针
 */
void switch_to(uint32_t* task_esp);

/**
 * 测试：执行一个任务
 */
void task(void);



#endif