/* include/Hydrangea/memory.h */

#ifndef I_H_MEMORY_H
#define I_H_MEMORY_H

#include <types.h>

/**
 * 用指定值填充内存区域
 * @param dst 目标内存起始地址
 * @param val 要填充的值（0-255）
 * @param count 要填充的字节数
 * @return 返回dst（便于链式调用）
 */
void memset(void* dst, int val, size_t count);

/**
 * 内存复制（不处理重叠）
 * @param dst 目标地址
 * @param src 源地址  
 * @param count 字节数
 */
void* memcpy(void* dst, const void* src, size_t count);

/**
 * 内存复制（处理重叠区域）
 * @param dst 目标地址
 * @param src 源地址
 * @param count 字节数
 */
void* memmove(void* dst, const void* src, size_t count);

/**
 * 内存比较
 * @param ptr1 内存块1
 * @param ptr2 内存块2  
 * @param count 比较字节数
 * @return 0=相等, <0 ptr1<ptr2, >0 ptr1>ptr2
 */
int memcmp(const void* ptr1, const void* ptr2, size_t count);



#endif