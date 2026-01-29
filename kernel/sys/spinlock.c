/* kernel/sys/spinlock.c */

#include <stddef.h>
#include <Tuix/spinlock.h>
#include <Tuix/panic.h>
#include <Tuix/process.h>
#include <arch/x86.h>

void init_lock(struct spinlock* lk, char* name)
{
    lk->name = name;
    lk->locked = 0;
    lk->cpu = 0;
}

void acquire(struct spinlock* lk)
{
    // 自旋锁持有期间绝对不切换进程
    pushcli();
    if(holding(lk)) // 防止编写时多次重入
        PANIC("acquire");

    // 尝试自旋,检查是否上锁
    while(xchg(&lk->locked, 1) != 0);

    __sync_synchronize();
    lk->cpu = c_cpu();
}

void release(struct spinlock* lk)
{
    if(!holding(lk))
        PANIC("release");

    __sync_synchronize();
    lk->cpu = 0;
    asm volatile("movl $0, %0" : "+m" (lk->locked) : );
    popcli();
}

void pushcli(void)
{
    int eflags;

    eflags = reade_flags();
    cli(); // 关中断
    if(c_cpu()->ncli == 0)
        c_cpu()->intena = eflags & FL_IF;

    c_cpu()->ncli++;
}

void popcli(void)
{
    if(reade_flags() & FL_IF)
        PANIC("popcli - interruptible");
    if(--c_cpu()->ncli < 0)
        PANIC("popcli");
    if(c_cpu()->ncli == 0 && c_cpu()->intena)
        sti();
}

int holding(struct spinlock* lock)
{
    int r;
    pushcli();
    r = lock->locked && lock->cpu == c_cpu();
    popcli();
    return r;
}
