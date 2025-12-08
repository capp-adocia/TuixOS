/* include/Tuix/sysconf.h */

#ifndef I_T_SYSCONF_H
#define I_T_SYSCONF_H

#include <stddef.h>

/* 核心数量 */
#ifndef MAX_CPUS
#define MAX_CPUS 1
#endif

/* 最大任务数 */
#ifndef MAX_TASKS
#define MAX_TASKS 256
#endif

/* 当前的cpuid */
extern uint8_t cpu_cur_id;

#endif
