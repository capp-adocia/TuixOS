/* include/Tuix/sysconf.h */

#ifndef I_T_SYSCONF_H
#define I_T_SYSCONF_H

#include <stddef.h>

/* 核心数量 */
#define MAX_CPUS 1
/* 最多进程数 */
#define MAX_PROC 64
/* 最大段数 */
#define MAX_SEGS 6
/* 内核栈大小 */
#define KSTACK_SIZE 4096
/* 每个进程可打开的文件数 */
#define OFILE_NUM 16
/* 系统可打开的文件数 */
#define SYS_OFILE_NUM 100
/* 最大活动i节点数 */
#define INODE_NUM 50
/* 最大主设备号 */
#define DEV_NUM 10
/* 文件系统根磁盘的设备号 */
#define ROOT_DEV 1
/* 文件系统操作可写的最大块数 */
#define MAX_OPBLOCKS 10
/* 磁盘日志中的最大数据块数 */
#define LOG_SIZE (MAXOPBLOCKS*3)
/* 磁盘块缓存大小 */
#define BUF_NUM (MAXOPBLOCKS*3)
/* 文件系统大小（块数）*/
#define FS_SIZE 1000

/* 当前的cpuid */
extern uint8_t cpu_id;

#endif
