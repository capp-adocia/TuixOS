/* include/Tuix/kalloc.h */

#ifndef I_T_KALLOC_H
#define I_T_KALLOC_H

#include <stddef.h>
#include <Tuix/mulitiboot2.h>

/**
 * 初始化指定范围的内存
 * @param vstart 起始虚拟地址
 * @param vend 结束虚拟地址
 */
void init_physical_memory(void* vstart, void* vend);

/**
 * 释放指定范围的内存
 * @param vstart 起始虚拟地址
 * @param vend 结束虚拟地址
 */
void free_range(void* vstart, void* vend);

/**
 * 页框分配
 * @return 空闲页的地址
 */
char* kalloc(void);

/**
 * 页框释放
 * @param ptr 释放指定位置的页
 */
void kfree(char* ptr);

/**
 * 返回空闲列表的大小
 * @return 空闲列表当前的字节数
 */
uint32_t kmem_size(void);

#endif
