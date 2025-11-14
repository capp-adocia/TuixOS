/* include/Silan/process.h */

#ifndef I_S_PROCESS_H
#define I_S_PROCESS_H

#include <stddef.h>
#include <Silan/queue.h>
#include <Silan/interrupts_types.h>

extern int task_started;

struct process_control_block
{
    // 任务状态信息
    uint32_t task_id;       // 任务ID
    uint32_t task_state;    // 运行状态 就绪、运行、阻塞等
    
    // 需要保存的寄存器
    uint32_t esp;
    uint32_t eip;
    uint32_t eflags;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t cs;
    uint32_t ds;
    uint32_t es;
    uint32_t ss;
    
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
 * 设置pcb的数据
 * @param pcb 初始化pcb
 * @param entry_point 任务的入口函数
 */
void setup_task_context(struct process_control_block* pcb, void (*entry_point)());

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
 * 处理上下文切换(在协作式任务切换使用)
 * @param prev 旧进程的pcb
 */
void context_switch(struct process_control_block* prev);

/**
 * 保存中断帧信息到当前pcb中
 * @param frame 中断帧
 */
void save_interrupt_frame(struct interrupt_frame* frame);

/**
 * 将新调度进程的pcb写入中断帧
 * @param frame 中断帧
 */
void restore_to_interrupt_frame(struct interrupt_frame* frame);

/**
 * 测试：执行任务
 */
void task_A(void);
void task_B(void);

#endif