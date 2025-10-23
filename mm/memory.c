/* mm/memory.c */

#ifndef MM_MEMORY_H
#define MM_MEMORY_H

#include <Hydrangea/memory.h>
#include <Hydrangea/screen.h>
#include <string.h>
#include <errno.h>

uint8_t phys_bitmap[TOTAL_PAGES / 8];
struct kernel_heap kheap;

void* memset(void* dst, int val, size_t count)
{
    if (dst == NULL) 
        return NULL;
    
    char* temp = (char*)dst;
    for (size_t i = 0; i < count; i++) {
        *temp++ = (char)val;
    }
    
    return dst;
}

void* memcpy(void* dst, const void* src, size_t count)
{
    if (dst == NULL || src == NULL)
        return NULL;
    
    if (count == 0)
        return dst;
    
    char* dst_t = (char*)dst;
    const char* src_t = (const char*)src;
    
    while (count--) {
        *dst_t++ = *src_t++;
    }
    return dst;
}

void* memmove(void* dst, const void* src, size_t count)
{
    if (dst == NULL || src == NULL)
        return NULL;
    
    if (count == 0 || dst == src)
        return dst;
    
    char* d = (char*)dst;
    const char* s = (const char*)src;
    
    if (d < s) {
        // 目标在源前面，正向拷贝
        while (count--) {
            *d++ = *s++;
        }
    } else {
        // 目标在源后面，反向拷贝从末尾开始
        d += count;
        s += count;
        while (count--) {
            *--d = *--s;
        }
    }
    return dst;
}

int memcmp(const void* ptr1, const void* ptr2, size_t count)
{
    if (ptr1 == NULL || ptr2 == NULL)
        return -1;  // 或者返回不相等
    
    // 如果count为0，认为相等
    if (count == 0)
        return 0;
    
    const unsigned char* p1 = (const unsigned char*)ptr1;
    const unsigned char* p2 = (const unsigned char*)ptr2;
    
    while (count-- > 0) {
        if (*p1 != *p2) {
            return (*p1 < *p2) ? -1 : 1;
        }
        p1++;
        p2++;
    }
    return 0;
}

void init_physical_memory(void)
{
    uint32_t used_end = 1 * 1024 * 1024; // 内核结束的位置
    uint32_t kernel_pages = used_end / PAGE_SIZE; // 256页
    uint32_t kernel_bytes = kernel_pages / 8; // 32字节
    // 探测内存大小
    // 先标记已使用
    memset(phys_bitmap, 0xFF, sizeof(phys_bitmap));
    // 标记后512B - 32B 大小可用
    memset(&phys_bitmap[kernel_bytes], 0x00, sizeof(phys_bitmap) - kernel_bytes);
}

uint32_t alloc_page(void)
{
    return alloc_pages(1);
}

uint32_t alloc_pages(size_t page_count)
{
    if (page_count == 0) 
        return 0;
    
    // 检查请求页数是否合理
    if (page_count > TOTAL_PAGES)
        return 0;
    
    size_t consecutive_free = 0;
    size_t start_page = 0;
    
    for (size_t i = 0; i < TOTAL_PAGES; i++) {
        uint32_t byte_index = i / 8;
        uint32_t bit_index = i % 8;
        
        if (!(phys_bitmap[byte_index] & (1 << bit_index))) {
            if (consecutive_free == 0) {
                start_page = i;
            }
            consecutive_free++;
            
            if (consecutive_free == page_count) {
                for (size_t j = start_page; j < start_page + page_count; j++) {
                    mark_page_used(j);
                }
                return start_page * PAGE_SIZE;
            }
        } else {
            consecutive_free = 0;
        }
    }
    
    return 0; // 内存不足
}

int alloc_pages_discrete(uint32_t* page_array, size_t page_count)
{
    if (page_array == NULL) 
        return -EINVAL;
    
    if (page_count == 0)
        return -EINVAL;
    
    for (size_t i = 0; i < page_count; i++) {
        page_array[i] = alloc_page();
        if (page_array[i] == 0) {
            // 分配失败，释放已分配的页
            for (size_t j = 0; j < i; j++) {
                free_page(page_array[j]);
            }
            return -ENOMEM;  // 内存不足
        }
    }
    return 0;
}

void free_page(uint32_t phys_addr)
{
    // 找到位图中的位置，并设置为0
    int page_index = phys_addr / PAGE_SIZE;
    int page_byte = page_index / 8;
    int page_offset = page_index % 8;
    mark_page_free(page_index);
}

void free_pages(uint32_t phys_addr, size_t page_count)
{
    int page_index = phys_addr / PAGE_SIZE;
    if (page_index + page_count > TOTAL_PAGES) {
        // 超出内存范围
        return -EMMBADADDR;
    }
    while(page_count--)
    {
        mark_page_free(page_index++);
    }
}

void free_pages_discrete(uint32_t* page_array, size_t page_count)
{
    for (size_t i = 0; i < page_count; i++)
    {
        if(page_array[i] != 0) free_page(page_array[i]);
    }
}

void get_memory_info(uint32_t* total, uint32_t* free)
{
    // 设置总页数
    *total = TOTAL_PAGES; // 存储的是二进制4096
    
    // 统计空闲页数
    uint32_t free_count = 0;
    for (size_t i = 0; i < TOTAL_PAGES; i++) {
        uint32_t byte_index = i / 8;
        uint32_t bit_index = i % 8;
        
        // 检查该位是否为0（空闲）
        if (!(phys_bitmap[byte_index] & (1 << bit_index))) {
            free_count++;
        }
    }
    *free = free_count;
}

bool page_is_free(uint32_t page_index)
{
    if(page_index >= TOTAL_PAGES) return false;

    int page_byte = page_index / 8;
    int page_offset = page_index % 8;
    return !(phys_bitmap[page_byte] & (1 << page_offset));
}

void mark_page_used(uint32_t page_index)
{
    int page_byte = page_index / 8;
    int page_offset = page_index % 8;
    phys_bitmap[page_byte] |= (1 << page_offset); // 按位或
}

void mark_page_free(uint32_t page_index)
{
    int page_byte = page_index / 8;
    int page_offset = page_index % 8;
    phys_bitmap[page_byte] &= ~(1 << page_offset); // 按位与
}

// 根据物理页分配情况初始化
void init_kernel_heap(void)
{
    // 从物理内存分配连续的堆空间
    uint32_t heap_phys = alloc_pages(HEAP_INIT_PAGES);
    if(!heap_phys)
    {
        kprintf(6, 0, "init_kernel_heap not alloc heap!");
        return;
    }
    // 初始化堆管理结构
    kheap.start_addr = (void*)heap_phys;
    kheap.end_addr = (void*)(heap_phys + HEAP_INIT_PAGES * PAGE_SIZE);
    kheap.total_size = HEAP_INIT_PAGES * PAGE_SIZE;
    kheap.used_size = 0;
    kheap.alloc_count = 0;
    kheap.free_count = 0;

    // 初始化第一个大空闲块（存放在1MB位置处）
    struct heap_block* first_block = HEAP_HEAD_PTR(heap_phys);
    first_block->size = kheap.total_size;
    first_block->used = 0; // 标记为空闲

    kprintf(6, 0, "kernel_heap: 0x%x -> 0x%x (%d KB)", 
       heap_phys, heap_phys + kheap.total_size, 
       kheap.total_size / 1024);
}

void* kmalloc(size_t size)
{
    // 这是实际需要的大小
    size_t total_size = size + HEAP_HEAD_SIZE;

    struct heap_block* current = HEAP_HEAD_PTR(kheap.start_addr);
    struct heap_block* end = HEAP_HEAD_PTR(kheap.end_addr);
    
    while ((char*)current < (char*)end)
    {
        if (!current->used && current->size >= total_size)
        {
            // 找到合适的空闲块，直接分配整个块
            current->used = 1;
            
            // 更新统计
            kheap.alloc_count++;
            kheap.used_size += size;
            
            // 返回数据区地址（跳过头部）
            return (void*)((char*)current + HEAP_HEAD_SIZE);
        }
        // 移动到下一个块
        current = HEAP_HEAD_PTR((char*)current + current->size);
    }
    
    // 内存不足
    return NULL;
}

void kfree(void* ptr)
{
    // 释放指定数据区地址的堆内存，先向前移动头部那么多字节，看看该区的信息
    struct heap_block* block = HEAP_HEAD_PTR((char*)ptr - HEAP_HEAD_SIZE);
    block->used = 0;

    kheap.free_count++;
    kheap.used_size -= (block->size - HEAP_HEAD_SIZE);
}

#endif