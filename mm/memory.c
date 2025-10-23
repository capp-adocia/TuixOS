/* mm/memory.c */

#ifndef MM_MEMORY_H
#define MM_MEMORY_H

#include <Hydrangea/memory.h>
#include <Hydrangea/screen.h>
#include <string.h>

uint8_t phys_bitmap[TOTAL_PAGES / 8];
struct kernel_heap kheap;

void memset(void* dst, int val, size_t count)
{
    char* temp = (char*)(dst); // 先进行类型转换
    for (size_t i = 0; i < count; i++)
    {
        *temp++ = (char*)val;
    }
    
}

// 不处理重叠区域
void* memcpy(void* dst, const void* src, size_t count)
{
    char* dst_t = (char*)(dst);
    char* src_t = (char*)(src);
    while (count--)
    {
        *dst_t++ = *src_t++;
    }
    return dst;
}

// 处理重叠区域
void* memmove(void* dst, const void* src, size_t count)
{
    char* d = dst;
    const char* s = src;
    
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
    const unsigned char* p1 = ptr1;
    const unsigned char* p2 = ptr2;
    
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
    if (page_count == 0) return 0;
    
    size_t consecutive_free = 0;
    size_t start_page = 0;
    
    for (size_t i = 0; i < TOTAL_PAGES; i++) {
        uint32_t byte_index = i / 8;
        uint32_t bit_index = i % 8;
        
        if (!(phys_bitmap[byte_index] & (1 << bit_index))) {
            // 找到空闲页
            if (consecutive_free == 0) {
                start_page = i;  // 记录连续空闲的开始位置
            }
            consecutive_free++;
            
            // 检查是否找到足够的连续页
            if (consecutive_free == page_count) {
                // 标记这些页为已使用
                for (size_t j = start_page; j < start_page + page_count; j++) {
                    mark_page_used(j);
                }
                return start_page * PAGE_SIZE;
            }
        } else {
            // 遇到已使用页，重置计数器
            consecutive_free = 0;
        }
    }
    
    return 0; // 内存不足或没有足够连续页
}

uint32_t alloc_pages_discrete(size_t page_size)
{
//    uint32_t* pages = /* 需要先有个地方存储页地址 */;
//     for (size_t i = 0; i < page_count; i++) {
//         pages[i] = alloc_page();  // 调用你的单页分配器
//         if (pages[i] == 0) {
//             // 分配失败，释放已分配的页
//             for (size_t j = 0; j < i; j++) {
//                 free_page(pages[j]);
//             }
//             return NULL;
//         }
//     }
//     return pages;
}

/* TODO:待补充要加入释放连续页和离散页 */
void free_page(uint32_t phys_addr)
{
    // 找到位图中的位置，并设置为0
    int page_index = phys_addr / PAGE_SIZE;
    int page_byte = page_index / 8;
    int page_offset = page_index % 8;
    mark_page_free(page_index);
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

}

void* kmalloc(size_t size)
{

}

void kfree(void* ptr)
{

}

#endif