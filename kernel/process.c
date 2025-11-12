/* kernel/process.c */

#include <Silan/process.h>
#include <Silan/serial.h>

// 分配一个4KB的任务栈
static uint32_t t_stack[1024];
static struct task_stack task_a;

void init_task(void)
{
    // 先为这个任务设置一个独立的栈
    // 让栈指针esp指向数组的末尾
    task_a.stack_limit = &t_stack[0];
    task_a.stack_start = &t_stack[1024];
    task_a.current_esp = &t_stack[1024];
    task_a.task_id = 1;

    // 告诉任务要从task_entry函数开始执行
    setup_task_context(&task_a, task);
    // 切换到任务A
    switch_to(task_a.current_esp);
    // serial_printf("\n任务结束，已经返回内核\n");
}

void setup_task_context(struct task_stack *task, void (*entry_point)())
{
    uint32_t* esp = task->stack_start;
    // 设置任务栈的数据
    *--esp = 0x202;                  // EFLAGS
    *--esp = 0x08;                   // CS
    *--esp = (uint32_t)entry_point;  // EIP
    *--esp = 0;                      // EAX
    *--esp = 0;                      // ECX
    *--esp = 0;                      // EDX
    *--esp = 0;                      // EBX
    *--esp = 0;                      // 占位符
    *--esp = 0;                      // EBP
    *--esp = 0;                      // ESI
    *--esp = 0;                      // EDI

    // 设置task的esp指向entry_point
    task->current_esp = esp;
}

void switch_to(uint32_t* new_esp)
{
    // 先压入内核栈，再切换到新栈弹出新栈的eip跳转，完成了内核到进程的切换
    __asm__ volatile(
        "cli\n"
        "pushl $0\n" // 先压入0占位
        "pushfl\n"
        "pusha\n"
        "movl 40(%%esp), %%eax\n" // 从ESP+40获取返回地址
        "movl %%eax, 36(%%esp)\n" // 存入ESP+36的EIP预留位置
        "mov %%esp, %0\n"
        "mov %1, %%esp\n"
        "popa\n"
        "iretl\n" // 弹出新栈的eip 前往新任务的入口地址
        : "=m"(task_a.kernel_esp) : "r"(new_esp) : "eax", "memory"
    );
}

void yield(void)
{
    // 利用内核的esp完成返回
    __asm__ volatile(
        "cli\n"
        "mov %0, %%esp\n"  // 切换回内核栈
        "popa\n"           // 弹出8个通用寄存器
        "popfl\n"          // 恢复EFLAGS
        "ret\n"            // 返回
        : :"r"(task_a.kernel_esp)
    );
}

void task(void)
{
    while (true)
    {
        serial_printf("A");
        break;
    }
    yield();
}