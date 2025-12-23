/* kernel/process.c */

#include <Tuix/sysconf.h>
#include <stddef.h>
#include <Tuix/queue.h>
#include <Tuix/process.h>
#include <Tuix/serial.h>
#include <Tuix/pic.h>
#include <Tuix/panic.h>
#include <Tuix/tss.h>

// 分配4KB的任务栈
static uint32_t ta_stack[1024]; // 用户栈
static uint32_t ta_kstack[1024]; // 内核栈
static uint32_t tb_stack[1024];
static uint32_t tb_kstack[1024];
struct process_control_block pcbs[MAX_TASKS];
struct process_control_block* pcb_curr;
struct queue r_queue;

void init_task(void)
{
    queue_init(&r_queue);
    // 初始化任务A
    pcbs[0].ustack.base = (uint32_t)ta_stack;
    pcbs[0].ustack.top = (uint32_t)ta_stack + sizeof(ta_stack);
    pcbs[0].kstack.base = (uint32_t)ta_kstack;
    pcbs[0].kstack.top = (uint32_t)ta_kstack + sizeof(ta_kstack);

    setup_task_context(&pcbs[0], task_A);
    enqueue(&r_queue, &(pcbs[0].ready_node));

    // 初始化任务B
    pcbs[1].ustack.base = (uint32_t)tb_stack;
    pcbs[1].ustack.top = (uint32_t)tb_stack + sizeof(tb_stack);
    pcbs[1].kstack.base = (uint32_t)tb_kstack;
    pcbs[1].kstack.top = (uint32_t)tb_kstack + sizeof(tb_kstack);
    setup_task_context(&pcbs[1], task_B);
    enqueue(&r_queue, &(pcbs[1].ready_node));
}

void switch_to(struct process_control_block *next)
{
    struct process_control_block* prev = pcb_curr;
    cpu_tss[cpu_cur_id].esp0 = next->kstack.top;
    context_switch(prev, next);
}

void context_switch(struct process_control_block *prev, struct process_control_block *next)
{
    __asm__ volatile(
        // 保存前一个任务的上下文
        "pushf\n"
        "push %%ebp\n"
        "push %%esi\n"
        "push %%edi\n"
        "push %%ebx\n"
        "push %%ds\n"
        "push %%es\n"
        "push %%fs\n"
        "push %%gs\n"
        
        // 保存当前栈指针到prev->ctx.esp
        "mov %%esp, %0\n"
        
        // 切换到下一个任务
        // 更新当前任务指针
        "mov %2, %%eax\n"    // next指针
        "mov %%eax, pcb_curr\n" // 这里更新为下一个pcb
        
        // 恢复栈指针
        "mov %1, %%esp\n"    // next->ctx.esp
        // 恢复上下文
        "pop %%gs\n"
        "pop %%fs\n"
        "pop %%es\n"
        "pop %%ds\n"
        "pop %%ebx\n"
        "pop %%edi\n"
        "pop %%esi\n"
        "pop %%ebp\n"
        "popf\n"
        
        // 跳转到下一个任务的EIP，利用中断保存到的信息
        "ret\n"
        : "=m"(prev->ctx.esp)
        : "m"(next->ctx.esp),
          "m"(next)
        : "eax", "memory"
    );
}

void launch_first_task()
{
    // 首次执行先调度一次
    schedule();
    // 这里额外设置一次，从内核切换到第一个任务，后续任务切换进行
    cpu_tss[cpu_cur_id].esp0 = pcb_curr->kstack.top;
    
    serial_printf("切换到用户态准备执行第一个任务...\n");

    __asm__ volatile(
        "pushl $0x23\n"          // SS (用户数据段)
        "pushl %0\n"             // ESP (用户栈指针)
        "pushl $0x202\n"         // EFLAGS (IF=1)
        "pushl $0x1B\n"          // CS (用户代码段)
        "pushl %1\n"             // EIP (任务入口)
        
        // 设置段寄存器
        "mov $0x23, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        
        // 切换到用户态
        "iret\n"
        : 
        : "r"(pcb_curr->ctx.esp),  // 用户栈
          "r"(pcb_curr->ctx.eip)   // 任务入口
        : "eax", "memory"
    );}

void setup_task_context(struct process_control_block* pcb, void (*entry_point)())
{   
    uint32_t* uesp = (uint32_t*)pcb->ustack.top;
    pcb->ctx.esp = (uint32_t)uesp;  // 用户栈指针
    pcb->ctx.eip = (uint32_t)entry_point;
    
    pcb->ctx.eax = 0;
    pcb->ctx.ecx = 0;
    pcb->ctx.edx = 0;
    pcb->ctx.ebx = 0;
    pcb->ctx.ebp = 0;
    pcb->ctx.esi = 0;
    pcb->ctx.edi = 0;
    pcb->ctx.ds = 0x23;
    pcb->ctx.es = 0x23;
    pcb->ctx.fs = 0;
    pcb->ctx.gs = 0;
    pcb->ctx.eflags = 0x202;
}

// 当进程使用了yield表示这个线程主动释放了cpu使用权，那么此时应该把它加入就绪队列，并从就绪队列中取出一个新任务
void yield(void)
{
    schedule();
}

void schedule(void)
{
    // 如果首次调度pcb_curr不存在从就绪队列中出队一个
    if(!pcb_curr)
    {
        struct list_head* next = dequeue(&r_queue);
        if (next)
            pcb_curr = container_of(next, struct process_control_block, ready_node);

        return;
    }
    // 先判断队头元素是否等于当前的pcb，相同则直接退出继续执行当前任务
    struct list_head* t = queue_peek(&r_queue);
    if(container_of(t, struct process_control_block, ready_node) == pcb_curr)
        return;

    // 将当前任务重新加入就绪队列
    enqueue(&r_queue, &(pcb_curr->ready_node));
    // 选择下一个任务
    struct list_head* next = dequeue(&r_queue);
    // 调度选择下一个任务后切换到该任务的上下文
    if(next)
        switch_to(container_of(next, struct process_control_block, ready_node));
}

void task_A(void)
{
    /* 启用定时器中断，注意在进入第一个任务时启用 */
    // enable_irq(IRQ_TIMER);
    __asm__ volatile("sti");

    volatile int count = 0;
    while (1)
    {
        uint32_t curesp;
        __asm__ volatile("mov %%esp, %0" : "=r"(curesp));
        if(curesp >= (uint32_t)pcbs[0].ustack.base && curesp <= (uint32_t)pcbs[0].ustack.top)
        {
            serial_printf("[There A at A ustack] ");
        }
        if(curesp >= (uint32_t)pcbs[1].ustack.base && curesp <= (uint32_t)pcbs[1].ustack.top)
        {
            serial_printf("[There A at B ustack] ");
        }
        serial_printf("A%d ", ++count);
        // for (volatile int i = 0; i < 5000000; i++);
        // yield();
    }
}

void task_B(void)
{
    volatile int count = 0;
    while (1)
    {
        uint32_t curesp;
        __asm__ volatile("mov %%esp, %0" : "=r"(curesp));
        if(curesp >= (uint32_t)pcbs[0].ustack.base && curesp <= (uint32_t)pcbs[0].ustack.top)
        {
            serial_printf("[There B at A ustack] ");
        }
        if(curesp >= (uint32_t)pcbs[1].ustack.base && curesp <= (uint32_t)pcbs[1].ustack.top)
        {
            serial_printf("[There B at B ustack] ");
        }

        serial_printf("B%d ", ++count);  // 应该输出 B1, B2, B3...
        // for (volatile int i = 0; i < 5000000; i++);
        // yield();
    }
}
