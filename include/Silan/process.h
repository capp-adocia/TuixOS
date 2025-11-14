/* include/Silan/process.h */

#ifndef I_S_PROCESS_H
#define I_S_PROCESS_H

#include <stddef.h>
#include <Silan/queue.h>

struct process_control_block
{
    // 任务状态信息
    uint32_t task_id;       // 任务ID
    uint32_t task_state;    // 运行状态 就绪、运行、阻塞等
    
    // 需要保存的寄存器
    uint32_t p_esp;
    uint32_t p_eip;
    uint32_t p_eflags;
    uint32_t p_edi;
    uint32_t p_esi;
    uint32_t p_ebp;
    uint32_t p_ebx;
    uint32_t p_edx;
    uint32_t p_ecx;
    uint32_t p_eax;
    
    // 调度信息
    uint32_t time_remaining;    // 剩余时间片
    uint32_t priority;          // 优先级
    struct task_stack
    {
        uint32_t *limit;        // 栈顶 低地址
        uint32_t *start;        // 栈底 高地址，栈起始的地址
        uint32_t *curr;  // 当前栈指针
    } stack; // 进程对应的栈信息
    
    struct list_head ready_node; // 就绪队列
};
// 就绪队列


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
 * @param old_esp 旧任务的栈指针
 * @param new_esp 新任务的栈指针
 */
void switch_to(uint32_t* old_esp, uint32_t* new_esp);

/**
 * 负责主动让出CPU
 */
void yield(void);

/**
 * 根据调度策略，选择下一个进程
 */
void schedule(void);

/**
 * 处理上下文切换
 * @param prev 旧进程的pcb
 */
void context_switch(struct process_control_block* prev);

/**
 * 测试：执行任务
 */
void task_A(void);
void task_B(void);

#endif