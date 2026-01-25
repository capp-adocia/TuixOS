/* tools/fs.h */

#ifndef T_FS_H
#define T_FS_H

#include <stddef.h>

#define OFILE_NUM     16    // 每个进程可打开的文件数
#define FILE_NUM      100   // 系统可打开的文件数
#define INODE_NUM     50    // 最大活动i节点数
#define DEV_NUM       10    // 最大主设备号
#define ROOT_DEV      1     // 文件系统根磁盘的设备号
#define MAX_ARG       32    // 最大执行参数数
#define MAX_OPBLOCKS  10    // 文件系统操作可写的最大块数
#define LOG_SIZE      (MAX_OPBLOCKS * 3)  // 磁盘日志中的最大数据块数
#define BUF_NUM       (MAX_OPBLOCKS * 3)  // 磁盘块缓存大小
#define FS_SIZE       1000  // 文件系统大小（块数）

#define ROOT_INO 1  // 根目录的inode num
#define B_SIZE 512  // block size

// 磁盘布局：
// [ 引导块1 | 超级块1 | 日志n | inode块n | 空闲位图n | 数据块 ]
//
// mkfs 计算超级块并构建初始文件系统。
// 超级块描述了磁盘布局：
struct superblock
{
    uint32_t size;         // 文件系统镜像的大小（块数）
    uint32_t nblocks;      // 数据块数量
    uint32_t ninodes;      // i节点数量
    uint32_t nlog;         // 日志块数量
    uint32_t logstart;     // 第一个日志块的块号
    uint32_t inodestart;   // 第一个i节点块的块号
    uint32_t bmapstart;    // 第一个空闲映射块的块号
};

#define DIRECT_NUM 12 // 直接地址数
// 间接地址一个块对应的inode编号
#define INDIRECT_NUM (B_SIZE / sizeof(uint32_t))
// 最多文件块数量: 12 + 一级索引表示的文件数(在一级索引中指针数量=文件数量)
#define MAX_FILE (DIRECT_NUM + INDIRECT_NUM)

// 磁盘上的 inode 结构体
struct dinode
{
  short type;                       // 文件类型
  short major;                      // 主设备号（仅 T_DEV）
  short minor;                      // 次设备号（仅 T_DEV）
  short nlink;                      // 文件系统中指向该 inode 的链接数
  uint32_t size;                    // 文件大小（字节）
  uint32_t addrs[DIRECT_NUM + 1];   // 数据块地址 12个直接 + 1个一级
};
// 每个块的 inode 数量。
#define IPB           (B_SIZE / sizeof(struct dinode))
// 包含 inode i 的块
#define IBLOCK(i, sb)     ((i) / IPB + sb.inodestart)

// 每个块的位图位数
#define BPB           (B_SIZE * 8)
// 包含块 b 位的空闲位图块
#define BBLOCK(b, sb) (b/BPB + sb.bmapstart)

// 目录是包含一系列 dirent 结构的文件。
#define DIR_SIZE 14

struct dirent
{
  uint16_t inum;
  char name[DIR_SIZE];
};
#endif
