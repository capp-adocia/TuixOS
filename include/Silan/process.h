/* include/Silan/process.h */

#ifndef I_S_PROCESS_H
#define I_S_PROCESS_H

#include <stddef.h>

struct task_stack
{
    // 栈内存管理
    uint32_t* stack_limit;     // 栈顶 低地址
    uint32_t* stack_start;    // 栈底 高地址，栈起始的地址  
    uint32_t* current_esp;  // 当前栈指针

    // 内核上下文（从任务切换回内核时需要）
    uint32_t kernel_esp;    // 内核栈指针
    uint32_t kernel_eip;    // 返回地址
    uint32_t kernel_eflags; // 内核标志
    
    // 通用寄存器（从内核切换回任务时需要）
    uint32_t kernel_edi;
    uint32_t kernel_esi;
    uint32_t kernel_ebp;
    uint32_t kernel_ebx;
    uint32_t kernel_edx;
    uint32_t kernel_ecx;
    uint32_t kernel_eax;
    
    // 任务状态信息
    uint32_t task_id;       // 任务ID
    uint32_t task_state;    // 运行状态
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
 * @param new_esp 新任务的栈指针
 */
void switch_to(uint32_t* new_esp);

/**
 * 任务结束，通知调度器
 */
void yield(void);

/**
 * 测试：执行一个任务
 */
void task(void);



#endif