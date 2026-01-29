/* include/Tuix/spinlock.h */

#ifndef I_T_SPINLOCK_H
#define I_T_SPINLOCK_H

#include <stddef.h>

// 自旋锁
struct spinlock
{
    uint32_t locked; // 锁是否已被持有？

    // 用于调试：
    char *name;      // 锁的名称。
    struct cpu *cpu; // 持有锁的CPU。
    uint32_t pcs[10];// 调用栈（一个程序计数器数组）
    // 锁定该锁时的调用顺序。
};

/**
 * 初始化锁
 * @parma lk 自旋锁
 * @parma name 名称
 */
void init_lock(struct spinlock* lk, char* name);

/**
 * 获得锁,使用cli关中断来保护临界区
 * @parma lk 自旋锁
 */
void acquire(struct spinlock* lk);

/**
 * 获得锁,使用cli关中断来保护临界区
 * @parma lk 自旋锁
 */
void release(struct spinlock* lk);

/**
 * 关中断并增加cli的引用计数,第一次调用是设置intena用于保存初始中断使能位的状态
 */
void pushcli(void);

/**
 * 减少cli的引用次数,一旦减到0并且intena需要开中断则才能开启
 */
void popcli(void);

/**
 * 检查当前cpu中是否持有锁
 * @return 返回是否持有
 */
int holding(struct spinlock* lock);


#endif
