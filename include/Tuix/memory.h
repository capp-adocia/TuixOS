/* include/Tuix/memory.h */

#ifndef I_T_MEMORY_H
#define I_T_MEMORY_H

#include <stddef.h>
#include <Tuix/mulitiboot2.h>

/* 物理页管理: 对于80386芯片只有4G地址空间，那么位图应该占用4GB/4KB/8 = 128KB */
#define MAX_ADDR    0xFFFFFFFF                     
#define PAGE_SHIFT  12
#define PAGE_SIZE   (1 << PAGE_SHIFT)              // 4KB = 4096B
#define TOTAL_PAGES (1 << (32 - PAGE_SHIFT))       // 1024K页 = 1048576页
#define BITMAP_SIZE (TOTAL_PAGES >> 3)             // 128KB = 131072B
/* 堆管理 */
#define HEAP_INIT_PAGES 59                         // 堆初始分配?页
#define HEAP_ALIGN      8                           // 8B对齐
#define MIN_BLOCK_SIZE (sizeof(struct heap_block) + HEAP_ALIGN) // 最小分配大小
#define HEAP_HEAD_SIZE (sizeof(struct heap_block)) // 堆头部大小
#define HEAP_HEAD_PTR(ptr) ((struct heap_block*)(ptr)) // 强制转换为堆头指针类型
/* 将来使用内核映射基址 */
#define KERNEL_VIRTUAL_BASE 0xC0000000

// 物理页位图数组
extern uint8_t phys_bitmap[BITMAP_SIZE];

// 堆块头
struct heap_block
{
    size_t size;        // 块大小 = 头部 + 数据区
    int used;           // 使用标志：1=已分配, 0=空闲
};
// 堆块数据
struct kernel_heap
{
    void* start_addr;   // 起始地址
    void* end_addr;     // 结束地址 
    size_t total_size;  // 整个堆空间总大小
    size_t used_size;   // 整个堆用户已使用量
    size_t alloc_count; // 用户分配次数
    size_t free_count;  // 用户释放次数
};
// 全局堆实例
extern struct kernel_heap kheap;

/**
 * 用指定值填充内存区域
 * @param dst 目标内存起始地址
 * @param val 要填充的值（0-255）
 * @param count 要填充的字节数
 * @return 返回dst（便于链式调用）
 */
void* memset(void* dst, int val, size_t count);

/**
 * 内存复制（不处理重叠）
 * @param dst 目标地址
 * @param src 源地址  
 * @param count 字节数
 * @return 目标指针
 */
void* memcpy(void* dst, const void* src, size_t count);

/**
 * 内存复制（处理重叠区域，可以保证安全）
 * @param dst 目标地址
 * @param src 源地址
 * @param count 字节数
 * @return 目标指针
 */
void* memmove(void* dst, const void* src, size_t count);

/**
 * 内存比较
 * @param ptr1 内存块1
 * @param ptr2 内存块2  
 * @param count 比较字节数
 * @return 当ptr1 < ptr2时返回-1, 当ptr1 > ptr2时返回1，相等返回0
 */
int memcmp(const void* ptr1, const void* ptr2, size_t count);

/**
 * 初始化物理内存（物理页）
 */
void init_physical_memory(void);

/**
 * 单页分配
 * @param errno 错误码
 * @return 物理地址
 */
uint32_t* alloc_page(int* err_code);

/**
 * 分配指定数量的物理页（连续分配）
 * @param page_count 分配的页数
 * @param err_code 错误码
 * @return 物理地址
 */
uint32_t* alloc_pages(size_t page_count, int* err_code);

/**
 * 分配指定数量的物理页（离散分配）
 * @param page_array 页地址数组
 * @param page_count 分配的页数
 * @return 错误码
 */
int alloc_pages_discrete(uint32_t* page_array, size_t page_count);

/**
 * 释放一个物理页
 * @param phys_addrs 物理地址
 */
void free_page(uint32_t phys_addr);

/**
 * 释放连续的物理页
 * @param page_count 释放的页数
 */
void free_pages(uint32_t phys_addr, size_t page_count);

/**
 * 释放离散的物理页
 * @param page_array 页地址数组
 * @param page_count 释放的页数
 */
void free_pages_discrete(uint32_t* page_array, size_t page_count);

/**
 * 获取内存信息
 * @param total 总共地址
 * @param free  空闲地址
 */
void get_memory_info(uint32_t* total, uint32_t* free);

/**
 * 检查页是否可用
 * @param page_index 页索引
 * @return 是否可用
 */
bool is_page_free(uint32_t page_index);

/**
 * 检查页是否使用
 * @param page_index 页索引
 * @return 是否使用
 */
bool is_page_used(uint32_t page_index);
/**
 * 标记页为已用（占用）
 * @param page_index 页索引
 */
void mark_page_used(uint32_t page_index);

/**
 * 标记页为可用（空闲）
 * @param page_index 页索引(页号，第几号页)
 */
void mark_page_free(uint32_t page_index);

/**
 * 初始化堆分配
 */
void init_kernel_heap(void);

/**
 * 堆分配
 * @param size 分配大小
 * @return 指向数据区的地址
 */
void* kmalloc(size_t size);

/**
 * 尝试合并零散的堆
 */
void kmalloc_compact(void);

/**
 * 堆释放
 * @param ptr 释放指定位置的堆（接收参数是数据区指针）
 */
void kfree(void* ptr);

#endif