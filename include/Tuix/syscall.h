/* include/Tuix/syscall.h */

#ifndef I_T_SYSCALL_H
#define I_T_SYSCALL_H

#include <stddef.h>
#include <Tuix/trap_frame.h>

// 系统调用号
#define SYS_FORK    1
#define SYS_EXIT    2   // 进程退出
#define SYS_WAIT    3
#define SYS_PIPE    4
#define SYS_READ    5
#define SYS_KILL    6
#define SYS_EXEC    7
#define SYS_FSTAT   8
#define SYS_CHDIR   9
#define SYS_DUP    10
#define SYS_GETPID 11
#define SYS_SBRK   12
#define SYS_SLEEP  13
#define SYS_UPTIME 14
#define SYS_OPEN   15
#define SYS_WRITE  16
#define SYS_MKNOD  17
#define SYS_UNLINK 18
#define SYS_LINK   19
#define SYS_MKDIR  20
#define SYS_CLOSE  21

/**
 * 系统调用分发函数，根据中断号来分发给不同系统调用
 * @param frame 陷阱帧
 */
void syscall_handlers(struct trap_frame* frame);

#endif
