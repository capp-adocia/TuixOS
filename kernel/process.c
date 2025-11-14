/* kernel/process.c */

#include <Silan/process.h>
#include <Silan/serial.h>

// 分配4KB的任务栈
static uint32_t ta_stack[1024];
static uint32_t tb_stack[1024];
struct process_control_block pcb_a;
struct process_control_block pcb_b;
struct process_control_block* pcb_curr;
static uint32_t kernel_esp = 0;
struct queue r_queue;

void init_task(void)
{
    queue_init(&r_queue);
    
    // 初始化任务A
    pcb_a.stack.limit = &ta_stack[0];
    pcb_a.stack.start = &ta_stack[1024];
    pcb_a.stack.curr = &ta_stack[1024];
    setup_task_context(&(pcb_a.stack), task_A);
    enqueue(&r_queue, &(pcb_a.ready_node));

    // 初始化任务B
    pcb_b.stack.limit = &tb_stack[0];
    pcb_b.stack.start = &tb_stack[1024];
    pcb_b.stack.curr = &tb_stack[1024];
    setup_task_context(&(pcb_b.stack), task_B);
    enqueue(&r_queue, &(pcb_b.ready_node));

    // 选择第一个任务
    struct list_head* node = dequeue(&r_queue);
    if(node)
        pcb_curr = container_of(node, struct process_control_block, ready_node);

    // 从内核切换到第一个任务，后续任务切换进行
    switch_to(&kernel_esp, pcb_curr->stack.curr);
}

void setup_task_context(struct task_stack *task, void (*entry_point)())
{
    uint32_t* esp = task->start;
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
    task->curr = esp;
}

void switch_to(uint32_t* old_esp, uint32_t* new_esp)
{
    // 先压入内核栈，再切换到新栈弹出新栈的eip跳转，完成了内核到进程的切换
    __asm__ volatile(
        "pushfl\n"                  // 保存内核的数据
        "cli\n"
        "pushl $0x08\n"
        "subl $4, %%esp\n"          // 为 EIP 预留空间
        "pusha\n"
        "leal 1f, %%eax\n"          // 获取返回地址
        "movl %%eax, 32(%%esp)\n"   // 写入预留的 EIP 位置
        "movl %%esp, (%0)\n"
        "mov %1, %%esp\n"
        "popa\n"
        "iretl\n"                   // 弹出新栈的eip 前往新任务的入口地址
        "1:\n"
        : 
        : "r"(old_esp), "r"(new_esp)
        : "eax", "memory"
    );
}

// 当进程使用了yield表示这个线程主动释放了cpu使用权，那么此时应该把它加入就绪队列，并从就绪队列中取出一个新任务
void yield(void)
{
    schedule();
}

void schedule(void)
{
    // 保存当前pcb
    struct process_control_block* prev = pcb_curr;
    // 将当前任务重新加入就绪队列
    enqueue(&r_queue, &(pcb_curr->ready_node));

    // 选择下一个任务
    struct list_head* node = dequeue(&r_queue);
    if(node)
        pcb_curr = container_of(node, struct process_control_block, ready_node);

    // 现在上下文切换可以单独处理
    context_switch(prev);
}

void context_switch(struct process_control_block* prev)
{
    // 与switch_to对称操作，类似于创建栈帧前往这个新创建的栈帧的esp
    __asm__ volatile(
        "pushfl\n"                  // 保存当前任务的数据
        "cli\n"
        "pushl $0x08\n"
        "subl $4, %%esp\n"          // 为 EIP 预留空间
        "pusha\n"
        "leal 1f, %%eax\n"          // 获取返回地址
        "movl %%eax, 32(%%esp)\n"   // 写入预留的 EIP 位置
        "mov %%esp, %0\n"           // 保存当前进程的esp
        "mov %1, %%esp\n"           // 切换回内核栈
        "popa\n"                    // 弹出8个通用寄存器
        "iretl\n"                   // 弹出下一个要前往的eip
        "1:\n"
        :"=m"(prev->stack.curr)
        :"r"(pcb_curr->stack.curr)
        : "eax", "memory"
    );
}

void task_A(void)
{
    int count = 0;
    while (1)
    {
        count++;
        serial_printf("A%d ", count);  // 应该输出 A1, A2, A3...
        for (volatile int i = 0; i < 50000000; i++);
        yield();
    }
}

void task_B(void)
{
    int count = 0;
    while (1)
    {
        count++;
        serial_printf("B%d ", count);  // 应该输出 B1, B2, B3...
        for (volatile int i = 0; i < 50000000; i++);
        yield();
    }
}
