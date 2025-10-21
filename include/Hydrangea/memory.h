/* include/Hydrangea/memory.h */

#ifndef I_H_MEMORY_H
#define I_H_MEMORY_H

#define PAGE_SIZE 4096
#define TOTAL_MEMORY 16 * 1024 * 1024
#define TOTAL_PAGES (TOTAL_MEMORY / PAGE_SIZE) // 4096页
/* 位图数组 */

#include <types.h>

extern uint8_t phys_bitmap[TOTAL_PAGES / 8]; // 分配512字节 管理4096个物理页

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

/**
 * 分配一个物理页
 */
uint32_t alloc_page(void);

/**
 * 释放一个物理页
 * @param phys_addrs 物理地址
 */
void free_page(uint32_t phys_addr);

/**
 * 获取内存信息
 * @param total 总共地址
 * @param free  空闲地址
 */
void get_memory_info(uint32_t* total, uint32_t* free);

/**
 * 检查页是否可用
 * @param page_index 页索引
 */
bool page_is_free(uint32_t page_index);

/**
 * 标记页为已用（占用）
 * @param page_index 页索引
 */
void mark_page_used(uint32_t page_index);

/**
 * 标记页为可用（空闲）
 * @param page_index 页索引
 */
void mark_page_free(uint32_t page_index);

#endif