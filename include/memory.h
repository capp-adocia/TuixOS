/* include/memory.h */

#ifndef I_MEMORY_H
#define I_MEMORY_H

#include <types.h>

/**
 * 用指定值填充内存区域
 * @param dst 目标内存起始地址
 * @param val 要填充的值（0-255）
 * @param count 要填充的字节数
 * @return 返回dst（便于链式调用）
 */
void memset(void* dst, int val, size_t count);

#endif