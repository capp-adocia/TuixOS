/* mm/memory.c */

#ifndef MM_MEMORY_H
#define MM_MEMORY_H

#include <Silan/memory.h>
#include <Silan/screen.h>
#include <string.h>
#include <errno.h>
#include <def.h>

uint8_t phys_bitmap[BITMAP_SIZE]; // 131072字节
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
    // 将位图的整个区域标记为已用
    memset(phys_bitmap, 0xFF, sizeof(phys_bitmap));
    // 找出各个条目是否可用
    uint32_t region_start_addr, region_end_addr, region_start_page, region_end_page;
    for (uint32_t i = 0; i < mem_info.memory_map_count; i++)
    {
        region_start_addr = (uint32_t)mem_info.memory_map[i].addr;
        region_end_addr = (uint32_t)(mem_info.memory_map[i].addr + mem_info.memory_map[i].len - 1);
        region_start_page = region_start_addr / PAGE_SIZE;
        region_end_page = region_end_addr / PAGE_SIZE;
        
        // 循环找到类型为1的代表可用区域将其标记为可用
        if(mem_info.memory_map[i].type == 1)
            // 注意这里循环条件应该是结束页面+1
            for (uint32_t cur_page = region_start_page; cur_page < region_end_page + 1; cur_page++)
                mark_page_free(cur_page);
        else
            for (uint32_t cur_page = region_start_page; cur_page < region_end_page + 1; cur_page++)
                mark_page_used(cur_page);
    }
    // 再将内核区域标记为已用
    uint32_t kstart_page = mem_info.kernel_start_addr / PAGE_SIZE;
    uint32_t kend_page = mem_info.kernel_end_addr / PAGE_SIZE;
    // 执行标记
    for (uint32_t cur_page = kstart_page; cur_page <= kend_page; cur_page++)
        mark_page_used(cur_page);
}

uint32_t* alloc_page(int* err_code)
{
    return alloc_pages(1, err_code);
}

uint32_t* alloc_pages(size_t page_count, int* err_code)
{
    if(err_code) *err_code = 0;

    // 页数为0
    if (page_count == 0)
    {
        if(err_code) *err_code = -1;
        return 0;
    }
    
    // 检查请求页数是否合理
    if (page_count > TOTAL_PAGES)
    {
        if(err_code) *err_code = -1;
        return 0;
    }
    
    size_t consecutive_free = 0;
    size_t start_page = 0;

    for (size_t i = 0; i < TOTAL_PAGES; i++)
    {
        if (is_page_free(i))
        {
            if (consecutive_free == 0)
                start_page = i;

            consecutive_free++;
            if (consecutive_free == page_count)
            {
                for (size_t j = start_page; j < start_page + page_count; j++)
                    mark_page_used(j);

                return (uint32_t*)(start_page * PAGE_SIZE);
            }
        }
        else
            consecutive_free = 0;
    }
    if(err_code) *err_code = -1;
    return 0; // 内存不足
}

int alloc_pages_discrete(uint32_t* page_array, size_t page_count)
{
    if (page_array == NULL) 
        return -EINVAL;
    
    if (page_count == 0)
        return -EINVAL;
    
    for (size_t i = 0; i < page_count; i++)
    {
        int error_code;
        page_array[i] = alloc_page(&error_code);
        if (error_code < 0)
        {
            // 分配失败，释放已分配的页
            for (size_t j = 0; j < i; j++)
                free_page(page_array[j]);
            
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
    // if (page_index + page_count > TOTAL_PAGES)
        // return -EMMBADADDR; // 超出内存范围

    while(page_count--)
        mark_page_free(page_index++);
}

void free_pages_discrete(uint32_t* page_array, size_t page_count)
{
    for (size_t i = 0; i < page_count; i++)
        if(page_array[i] != 0) free_page(page_array[i]);
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

bool is_page_free(uint32_t page_index)
{
    if(page_index >= TOTAL_PAGES) return false;

    int page_byte = page_index / 8;
    int page_offset = page_index % 8;
    
    return !(phys_bitmap[page_byte] & (1 << page_offset));
}

bool is_page_used(uint32_t page_index)
{
    if(page_index >= TOTAL_PAGES) return true;

    int page_byte = page_index / 8;
    int page_offset = page_index % 8;
    return (phys_bitmap[page_byte] & (1 << page_offset));
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

static inline uint32_t* phys_to_virt(uint32_t* phys)
{
    return phys; // 现在直接返回，将来要改
}

// 根据物理页分配情况初始化
void init_kernel_heap(void)
{
    // 从物理内存分配连续的堆空间
    int error_code;
    uint32_t* heap_phys = alloc_pages(HEAP_INIT_PAGES, &error_code);
    // TODO: 将来换成恒等映射后要改
    uint32_t* heap_virt = phys_to_virt(heap_phys);
    if(error_code < 0)
    {
        kprintf(3, 0, "init_kernel_heap not alloc heap!");
        return;
    }
    // 初始化堆管理结构 [start, end)
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

    kprintf(3, 0, "---kernel_heap: 0x%x -> 0x%x (%d KB)---", 
       heap_phys, heap_phys + kheap.total_size - 1, 
       kheap.total_size / 1024);
}

static void* try_allocate_block(struct heap_block* start, size_t total_size, size_t user_size)
{
    struct heap_block* current = start;
    struct heap_block* end = HEAP_HEAD_PTR(kheap.end_addr);
    
    while((char*)current < (char*)end)
    {
        if(!current->used && current->size >= total_size)
        {
            // 检查是否可以分割
            if(current->size >= total_size + HEAP_HEAD_SIZE + 8)
            {
                // 分割块
                size_t remaining_size = current->size - total_size;
                current->size = total_size;
                current->used = 1;
                
                struct heap_block* new_block = HEAP_HEAD_PTR((char*)current + total_size);
                new_block->size = remaining_size;
                new_block->used = 0;
            }
            else current->used = 1; // 使用整个块

            
            kheap.alloc_count++;
            kheap.used_size += user_size;
            return (void*)((char*)current + HEAP_HEAD_SIZE);
        }
        
        current = HEAP_HEAD_PTR((char*)current + current->size);
    }
    return NULL;
}

void* kmalloc(size_t size)
{
    // 这是系统实际需要的大小
    size_t total_size = size + HEAP_HEAD_SIZE;
    struct heap_block* current = HEAP_HEAD_PTR(kheap.start_addr);
    if(total_size % 8 != 0) // 需要按字节对齐分配
        total_size = ((total_size + 7) / 8) * 8;
        
    // 尝试第一次分配
    void* data_addr = try_allocate_block(current, total_size, size);

    // 如果分配失败，则尝试合并成更大的块，再进行分配
    if(!data_addr)
    {
        kmalloc_compact();
        data_addr = try_allocate_block(current, total_size, size);
    }
    
    return data_addr;
}

void kmalloc_compact(void)
{
    // 合并零散的内存空间
    struct heap_block* cur_ptr = HEAP_HEAD_PTR(kheap.start_addr);
    struct heap_block* end = HEAP_HEAD_PTR(kheap.end_addr);
    
    while ((char*)cur_ptr < (char*)end)
    {
        if (cur_ptr->used == 0) {
            // 持续合并后续的空闲块
            struct heap_block* next_ptr = HEAP_HEAD_PTR((char*)cur_ptr + cur_ptr->size);
            
            while ((char*)next_ptr < (char*)end && next_ptr->used == 0) {
                cur_ptr->size += next_ptr->size;
                next_ptr = HEAP_HEAD_PTR((char*)cur_ptr + cur_ptr->size);
            }
        }
        
        // 移动到下一个块
        cur_ptr = HEAP_HEAD_PTR((char*)cur_ptr + cur_ptr->size);
    }
}

void kfree(void* ptr)
{
    if (ptr == NULL) return;
    
    struct heap_block* block = HEAP_HEAD_PTR((char*)ptr - HEAP_HEAD_SIZE);
    size_t data_size = block->size - HEAP_HEAD_SIZE;
    
    #ifdef DEBUG
    // 用特殊值填充，便于调试识别已释放内存
    memset(ptr, 0xDE, data_size);
    #endif
    
    block->used = 0;
    kheap.free_count++;
    kheap.used_size -= data_size;
}

#endif