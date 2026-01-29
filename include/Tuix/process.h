/* include/Tuix/process.h */

#ifndef I_T_PROCESS_H
#define I_T_PROCESS_H

#include <Tuix/queue.h>
#include <Tuix/trap_frame.h>
#include <Tuix/sysconf.h>
#include <Tuix/gdt.h>
#include <Tuix/file.h>
#include <Tuix/spinlock.h>

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
    enum proc_state state;        // 进程状态(默认为UNUSED)
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
    // 系统打开文件表
    struct file* ofile[OFILE_NUM];
    // 睡眠锁指针,如果非零，则在chan上睡眠
    void* chan;
    // 就绪队列
    struct list_head ready_node; // NOTE:暂时未使用
};

// cpu状态，当前默认用单核
struct cpu
{
    struct gdt_entry gdt[MAX_SEGS];     // x86 全局描述符表
    struct context *scheduler;          // 在这里使用 swtch() 进入调度器(内核的栈esp指针)
    volatile uint32_t started;          // CPU 是否已启动？
    struct proc *proc;                  // 当前运行在此 CPU 上的进程或为空
    struct tss_entry ts;                // 任务状态信息
    int ncli;                           // pushcli 嵌套的深度。
    int intena;                         // 在 pushcli 前中断是否已启用？
    int pid;                            // 进程号
};

extern struct cpu cpus[MAX_CPUS];

// 就绪队列

/**
 * 第一次执行用户进程的流程：
 * 1. 调用初始化用户程序，分配进程所需要的内核栈、内存
 * 2. 将用户代码移动到用户空间
 * 3. 将低地址0-4096映射到用户空间映射
 * 4. 设置好进程tf的必要信息
 * 5. main函数中执行scheduler函数，开始调度
 * 6. 调度开始先选择一个处于可运行状态的进程
 * 7. 再切换到用户页目录基址，执行switch_to函数，切换新旧进程的context给esp(注意这里context其实指的就是内核栈的esp)
 * 8. 完成如何后，回到下一条指令，恢复内核页目录基址
 */

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
 * ----------------------- <-stack top
 * |  struct trap_frame  |
 * -----------------------
 * |    eip = trap_ret   |
 * -----------------------
 * |    eip = fork_ret   |
 * -----------------------
 * |  struct  context    |
 * ----------------------- <-proc->ctx
 * |       ......        |
 * ----------------------- <-stack bottom
 */
/**
 * 配置进程的内核栈细节：
 * 1. 从栈顶开始分配trap_frame大小的空间给它
 * 2. 再分配4B给放跳转地址trap_ret
 * 3. 最后分配context大小的空间，进程的上下文指针指向这里
 * 4. context里指定eip为fork_ret地址
 * 详细说明如果执行第一个进程:
 * 1. 首先利用switch_to切换，将当前esp指向新进程的ctx
 * 2. 执行ret将eip弹出，前往fork_ret函数
 * 3. fork_ret再弹出eip=trap_ret，前往trap_ret
 * 4. trap_ret将会弹出所有trap_frame里面的东西，最后前往我们在tf里指定的虚拟地址0
 * 5. 到达虚拟地址0，开始执行用户程序。
 */
/**
 * 配置进程的内核栈
 * @param proc 进程指针
 * @return 进程指针
 */
struct proc* config_proc_kstack(struct proc* proc);

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
void launch_first_proc(void);

/**
 * 负责主动让出CPU
 */
void yield(void);

/**
 * 根据调度策略，选择下一个进程
 */
void schedule(void);

/**
 * 执行进行调度，选择一个新的进程
 */
void sched(void);

/**
 * 获取当前运行的cpu上下文
 * @param 返回cpu实例
 */
static inline struct cpu* c_cpu(void)
{
    return &cpus[cpu_id];
}

/**
 * 睡眠,当前进程放弃cpu,切换到其他就绪进程
 * @param chan
 * @param lk 自旋锁
 */
void sleep(void* chan, struct spinlock* lk);

/**
 * 叫醒处在睡眠的进程
 * @param chan 睡眠锁
 */
void wakeup(void* chan);

#endif
