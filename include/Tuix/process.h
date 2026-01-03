/* include/Tuix/process.h */

#ifndef I_T_PROCESS_H
#define I_T_PROCESS_H

#include <Tuix/queue.h>
#include <Tuix/trap_frame.h>
#include <Tuix/sysconf.h>
#include <Tuix/gdt.h>

enum proc_state { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

struct context // 用于进程间的切换
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebx;
    uint32_t ebp;
    uint32_t eip;
};

struct proc
{
    // 任务状态信息
    int pid;                      // 任务ID
    enum proc_state state;        // 进程状态
    uint32_t size;                // 进程内存大小（字节）
    pde_t* pgdir;                 // 页目录基地址
    struct proc *parent;          // 父进程
    struct trap_frame *tf;        // 当前系统调用的陷阱帧里面有用户栈信息
    // 内核调度 寄存器上下文
    struct context* ctx;          // 实际上保存的是内核栈寄存器信息指针
    // 调度信息
    int killed;                   // 如果非零，表示已被杀死
    char name[16];                // 进程名称
    // 内核栈
    char* kstack;
    uint32_t kstack_size;
    // 就绪队列
    struct list_head ready_node;
};

// cpu状态，当前默认用单核
struct cpu
{
    struct gdt_entry gdt[MAX_SEGS];     // x86 全局描述符表
    struct context *scheduler;          // 在这里使用 swtch() 进入调度器
    volatile uint32_t started;          // CPU 是否已启动？
    struct proc *proc;                  // 当前运行在此 CPU 上的进程或为空
    struct tss_entry ts;                // 任务状态信息
};

extern struct cpu cpus[MAX_CPUS];

// 就绪队列

/* 任务数组 */
// extern struct proc pcbs[MAX_TASKS];

/**
 * 初始化第一个用户程序
 */
void init_user(void);

/**
 * 分配进程所需要的内存
 * @return 内存地址
 */
struct proc* alloc_process(void);

/**
 * 配置进程的状态
 * @param proc 进程指针
 * @return 进程指针
 */
struct proc* config_proc(struct proc* proc);

/**
 * 任务切换
 * @param $1 旧进程的上下文二级指针
 * @param $2 新进程的上下文指针
 * PS: 在switch.asm有对应实现
 */
void switch_to(struct context**, struct context*);

/**
 * 启动第一个用户进程
 */
void launch_first_proc();

/**
 * 负责主动让出CPU
 */
void yield(void);

/**
 * 根据调度策略，选择下一个进程
 */
void schedule(void);

#endif
