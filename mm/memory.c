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
    // 获取总共有多大
    *total = TOTAL_PAGES;
    int count = 0;
    for (size_t i = 0; i < TOTAL_PAGES; i++)
    {
        int page_index = i / 8;
        int page_offset = i % 8;
        if(phys_bitmap[page_index] & (1 << page_index))
        {
            count++;
        }
    }
    kprint(int_to_str(count), 4, 10);
    
}

bool page_is_free(uint32_t page_index)
{

}

void mark_page_used(uint32_t page_index)
{

}

void mark_page_free(uint32_t page_index)
{

}

#endif