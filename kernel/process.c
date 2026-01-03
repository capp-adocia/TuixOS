/* kernel/process.c */

#include "Tuix/gdt.h"
#include "Tuix/mmu.h"
#include "Tuix/vm.h"
#include <Tuix/sysconf.h>
#include <stddef.h>
#include <Tuix/queue.h>
#include <Tuix/process.h>
#include <Tuix/serial.h>
#include <Tuix/pic.h>
#include <Tuix/panic.h>
#include <Tuix/kalloc.h>
#include <string.h>

static struct proc* proc_first; // 第一个进程
int next_pid = 1; // pid
struct cpu cpus[MAX_CPUS];
extern void trap_ret(void);

/* 进程表 */
struct
{
    struct proc proc[MAX_PROC];
} ptable;

// 新进程第一个执行的函数
static void fork_ret(void)
{
    static int first = 1;
    if (first)
    {

    }
    // 新进程都会执行这个函数，返回后弹出栈顶指针eip = trap_ret
}

void init_user(void)
{
    struct proc* p;

    p = alloc_process();
    // 填充进程用户上下文信息
    proc_first = p;
    p->pgdir = setup_kvm(); // 创建新的页表映射给进程
    if(p->pgdir == 0)
        PANIC("userinit: out of memory?");
    // init_uvm(p->pgdir, ); // TODO
    p->size = PGSIZE;
    memset(p->tf, 0, sizeof(*(p->tf)));
    p->tf->cs = USR_CS | USR_DPL;
    p->tf->ds = USR_DS | USR_DPL;
    p->tf->es = p->tf->ds;
    p->tf->ss = p->tf->ds;
    p->tf->eflags = FL_IF;
    p->tf->esp = PGSIZE; // 注意进程内存空间只有1页，而从地址0开始，那么栈顶就在0+PGSIZE这里
    p->tf->eip = 0; // 从虚拟地址0开始
    strncpy(p->name, "initcode", sizeof(p->name));
    p->state = RUNNABLE; // 就绪状态
}

struct proc* alloc_process(void)
{
    struct proc* p;

    for(int i = 0;i < MAX_PROC;i++)
    {
        if(ptable.proc[i].state == UNUSED)
        {
            return config_proc(&(ptable.proc[i]));
        }
    }
    return 0;
}


struct proc* config_proc(struct proc* proc)
{
    char* sp; // 栈指针
    proc->state = EMBRYO;
    proc->pid = next_pid++;
    // 分配内核栈
    // 如果分配失败了
    proc->kstack = kalloc();
    if(proc->kstack == 0)
    {
        proc->state = UNUSED;
        return 0;
    }
    // 分配成功后将sp指向栈顶
    sp = proc->kstack + KSTACK_SIZE;
    sp -= sizeof(*(proc->tf)); // 预留陷阱栈这么大的空间
    
    // 预留4字节刚好能放一个32位地址，放入trap_ret的地址
    // trap_ret其实就是trap函数的下半部分
    sp -= sizeof(uint32_t);
    *(uint32_t*)sp = (uint32_t)trap_ret; // 新进程执行的第二个函数
    
    // 再放入内核栈上下文信息
    sp -= sizeof(*(proc->ctx));
    proc->ctx = (struct context*)sp;
    memset(proc->ctx, 0, sizeof(*(proc->ctx)));
    proc->ctx->eip = (uint32_t)fork_ret;
    return proc;
}

void launch_first_proc()
{
    struct proc *p;
    struct cpu *c = cpus;
    c->proc = 0;

    while(true)
    {
        __asm__ volatile("sti");

        // 遍历进程表，找到可运行的进程
        for(int i = 0;i < MAX_PROC;i++)
        {
            if(ptable.proc[i].state != RUNNABLE)
            {
                continue;
            }
            p = &(ptable.proc[i]);
            // 找到后，设置进程的状态
            c->proc = p;
            // 切换到用户虚拟页表
            switch_uvm(p);
            p->state = RUNNING;
            // 进行上下文切换，执行这个函数后，后面都不会再返回了，除非已经直接完成
            switch_to(&(c->scheduler), p->ctx);
            // 切换回内核页表，因为已经到内核态了
            switch_kvm();
            c->proc = 0; // 执行到这里用户进程已经完成了
        }
    }
}

// 当进程使用了yield表示这个线程主动释放了cpu使用权，那么此时应该把它加入就绪队列，并从就绪队列中取出一个新任务
void yield(void)
{
    schedule();
}

void schedule(void)
{
    // // 如果首次调度pcb_curr不存在从就绪队列中出队一个
    // if(!pcb_curr)
    // {
    //     struct list_head* next = dequeue(&r_queue);
    //     if (next)
    //         pcb_curr = container_of(next, struct proc, ready_node);
    //
    //     return;
    // }
    // // 先判断队头元素是否等于当前的pcb，相同则直接退出继续执行当前任务
    // struct list_head* t = queue_peek(&r_queue);
    // if(container_of(t, struct proc, ready_node) == pcb_curr)
    //     return;
    //
    // // 将当前任务重新加入就绪队列
    // enqueue(&r_queue, &(pcb_curr->ready_node));
    // // 选择下一个任务
    // struct list_head* next = dequeue(&r_queue);
    // // 调度选择下一个任务后切换到该任务的上下文
    // if(next)
    //     switch_to(container_of(next, struct proc, ready_node));
}
