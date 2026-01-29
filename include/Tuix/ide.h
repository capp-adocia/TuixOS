/* include/Tuix/ide.h */

#ifndef I_T_IDE_H
#define I_T_IDE_H

#include <stddef.h>

/**
 * 对ide驱动进行初始化
 */
void init_ide(void);

/**
 * 磁盘操作完成后触发中断执行此函数
 */
void ide_handler(void);

#endif
