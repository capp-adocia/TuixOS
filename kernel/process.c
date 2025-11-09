/* kernel/process.c */

#include <Silan/process.h>
#include <Silan/serial.h>

// 分配一个4KB的任务栈
static uint32_t t_stack[1024];

void init_task(void)
{
    // 先为这个任务设置一个独立的栈
    // 让栈指针esp指向数组的末尾
    struct task_stack task_a;
    task_a.stack_limit = &t_stack[0];
    task_a.stack_start = &t_stack[1024];
    task_a.current_esp = &t_stack[1024];

    // 告诉任务要从task_entry函数开始执行
    setup_task_context(&task_a, task);
    // 切换到任务A
    switch_to(task_a.current_esp);
}

void setup_task_context(struct task_stack *task, void (*entry_point)())
{
    uint32_t* esp = task->stack_start;
    // 设置任务栈的数据
    *--esp = 0;
    *--esp = 0;
    *--esp = 0;
    *--esp = 0;
    *--esp = 0;
    *--esp = 0;
    *--esp = 0;
    *--esp = 0;
    *--esp = 0x202;
    *--esp = (uint32_t)entry_point;

    // 设置task的esp指向entry_point
    task->current_esp = esp;
}

void switch_to(uint32_t* task_esp)
{
    __asm__ volatile(
        "mov %0, %%esp\n"
        "popa\n"
        "iretl\n"
        :
        : "r"(*task_esp)
    );
}

void task(void)
{
    while (true)
    {
        serial_printf("A");
    }
}