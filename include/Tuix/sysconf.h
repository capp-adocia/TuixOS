/* include/Tuix/sysconf.h */

#ifndef I_T_SYSCONF_H
#define I_T_SYSCONF_H

#include <stddef.h>

/* 核心数量 */
#ifndef MAX_CPUS
#define MAX_CPUS 1
#endif

/* 最多进程数 */
#ifndef MAX_PROC
#define MAX_PROC 64
#endif

/* 最大段数 */
#ifndef MAX_SEGS
#define MAX_SEGS 6
#endif

/* 内核栈大小 */
#ifndef KSTACK_SIZE
#define KSTACK_SIZE 4096
#endif

/* 当前的cpuid */
extern uint8_t cpu_id;

#endif
