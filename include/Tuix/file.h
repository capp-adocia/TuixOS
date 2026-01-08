/* include/Tuix/file.h */

#ifndef I_T_FILE_H
#define I_T_FILE_H

#include <stddef.h>

#define O_RONLY   0x000 // 以只读方式打开文件
#define O_WONLY   0x001 // 以只写方式打开文件
#define O_RWRIT   0x002 // 以读写方式打开文件
#define O_CREAT   0x200 // 以创建方式打开文件

struct file
{
    int ref;
    char readable;
    char writeable;
    struct inode* ip;
    uint32_t offset;
};

/* 磁盘上inode的副本 */
struct inode
{
    uint32_t dev;
    uint32_t num;
    int ref;
    // lock锁?
    int valid; // ？该inode是否有效

    short type;
    short major;
    short minor;
    short nlink;
    uint32_t size;
    // uint32_t addrs[]
};

#endif
