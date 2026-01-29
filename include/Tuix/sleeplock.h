/* include/Tuix/sleeplock.h */

#ifndef I_T_SLEEPLOCK_H
#define I_T_SLEEPLOCK_H

#include <stddef.h>
#include <Tuix/spinlock.h>

// 进程的睡眠锁
struct sleeplock {
    uint32_t locked;       // 锁是否被占用？
    struct spinlock lk; // 保护此睡眠锁的自旋锁

    // 用于调试：
    char *name;        // 锁的名称
    int pid;           // 持有锁的进程
};

/**
 * 初始化锁
 * @parma lk 自旋锁
 * @parma name 名称
 */
void init_sleeplock(struct sleeplock* lk, char* name);

/**
 * 获得锁
 * @parma lk 自旋锁
 */
void acquire_sleep(struct sleeplock* lk);

/**
 * 释放锁
 * @parma lk 自旋锁
 */
void release_sleep(struct sleeplock* lk);

/**
 * 是否占有锁
 * @parma lk 自旋锁
 */
int holding_sleep(struct sleeplock* lk);

#endif
