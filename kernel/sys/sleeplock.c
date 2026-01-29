/* kernel/sys/sleeplock.c */

#include <Tuix/process.h>
#include <Tuix/sleeplock.h>
#include <Tuix/panic.h>
#include <arch/x86.h>

void init_sleeplock(struct sleeplock* lk, char* name)
{
    init_lock(&lk->lk, "sleep lock");
    lk->name = name;
    lk->locked = 0;
    lk->pid = 0;
}

void acquire_sleep(struct sleeplock* lk)
{
    acquire(&lk->lk);
    while(lk->locked)
    {
        sleep(lk, &lk->lk); // sleep里面释放锁后切换进程,当恢复时又重新获得锁
    }
    lk->locked = 1;
    lk->pid = c_cpu()->pid;
    release(&lk->lk);
}

void release_sleep(struct sleeplock* lk)
{

}

int holding_sleep(struct sleeplock* lk)
{

}
