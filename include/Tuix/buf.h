/* include/Tuix/buf.h */

#ifndef I_T_BUF_H
#define I_T_BUF_H

#include <Tuix/fs.h>
#include <Tuix/sleeplock.h>

struct buf
{
    int flags;
    uint32_t dev;
    uint32_t blockno;
    struct sleeplock lock;
    uint32_t refcnt; // 引用计数
    struct buf *prev; // LRU cache list
    struct buf *next;
    struct buf *qnext; // disk queue
    uint8_t data[B_SIZE];
};

#define B_VALID 0x2  // 缓冲区已从磁盘读取
#define B_DIRTY 0x4  // 缓冲区需要写入磁盘

#endif
