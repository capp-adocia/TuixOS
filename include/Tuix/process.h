/* include/Tuix/process.h */

#ifndef I_T_PROCESS_H
#define I_T_PROCESS_H

#include <Tuix/queue.h>
#include <Tuix/ptrace.h>
#include <Tuix/sysconf.h>

struct process_control_block
{
    // 任务状态信息
    uint32_t id;       // 任务ID
    uint8_t state;    // 运行状态 就绪、运行、阻塞等
    
    // 寄存器上下文
    struct cpu_context
    {
        uint32_t esp, eip, ebp;
        uint32_t eax, ebx, ecx, edx;
        uint32_t esi, edi;
        uint32_t ds, es, fs, gs;
    } ctx;
    // 调度信息
    uint32_t time_remaining;    // 剩余时间片
    uint32_t priority;          // 优先级
    // 栈
    struct kernel_stack
    {
        uint32_t base;
        uint32_t top;
    } ustack, kstack; // 进程对应的用户栈以及内核栈信息
    
    struct list_head ready_node; // 就绪队列
};
// 就绪队列

/* 任务数组 */
extern struct process_control_block pcbs[MAX_TASKS];

/**
 * 初始化任务
 */
void init_task(void);

/**
 * 任务切换
 * @param next 下一个任务
 */
void switch_to(struct process_control_block *next);

/**
 * 任务上下文切换
 * @param prev 上一个任务
 * @param next 下一个任务
 */
void context_switch(struct process_control_block *prev, struct process_control_block *next);

/**
 * 启动第一个任务
 */
void launch_first_task();

/**
 * 设置pcb的数据
 * @param pcb 初始化pcb
 * @param entry_point 任务的入口函数
 */
void setup_task_context(struct process_control_block* pcb, void (*entry_point)());

/**
 * 负责主动让出CPU
 */
void yield(void);

/**
 * 根据调度策略，选择下一个进程
 */
void schedule(void);

/**
 * 测试：执行任务
 */
void task_A(void);
void task_B(void);

#endif
