/* mm/memory.c */

#ifndef MM_MEMORY_H
#define MM_MEMORY_H

#include <Hydrangea/memory.h>
#include <Hydrangea/screen.h>
#include <string.h>

uint8_t phys_bitmap[TOTAL_PAGES / 8];

void memset(void* dst, int val, size_t count)
{
    uint8_t* temp = (uint8_t*)(dst);
    for (size_t i = 0; i < count; i++)
    {
        *temp++ = (uint8_t)val;
    }
    
}

void* memcpy(void* dst, const void* src, size_t count)
{

}

void* memmove(void* dst, const void* src, size_t count)
{

}

int memcmp(const void* ptr1, const void* ptr2, size_t count)
{
    
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
    // 从位图中一页一页找空闲的，如果该位为0则找到空闲的物理页
    for (size_t i = 0; i < TOTAL_PAGES; i++)
    {
        uint32_t page_index = i / 8;
        uint32_t page_offset = i % 8;
        if(!(phys_bitmap[page_index] & (1 << page_offset)))
        {
            // 找到了，然后标记为已使用
            phys_bitmap[page_index] |= (1 << page_offset); // 按位或

            return i * PAGE_SIZE;
        }
    }
    return 0; // 内存不足
}

void free_page(uint32_t phys_addr)
{
    // 找到位图中的位置，并设置为0
    int page_index = phys_addr / PAGE_SIZE;
    int page_byte = page_index / 8;
    int page_offset = page_index % 8;
    phys_bitmap[page_byte] &= ~(1 << page_offset);
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

}

void mark_page_free(uint32_t page_index)
{

}

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