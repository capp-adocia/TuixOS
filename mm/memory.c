/* mm/memory.c */

#ifndef MM_MEMORY_H
#define MM_MEMORY_H

#include <Silan/memory.h>
#include <Silan/screen.h>
#include <string.h>
#include <errno.h>
#include <def.h>

uint8_t phys_bitmap[32768 / 8]; // 4096字节
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
    // 找出最大物理地址
    uint32_t max_physical_addr = 0;
    
    for (uint32_t i = 0; i < mem_info.memory_map_count; i++)
    {
        uint32_t region_end = (uint32_t)(mem_info.memory_map[i].addr + mem_info.memory_map[i].len);
        if (region_end > max_physical_addr)
        {
            max_physical_addr = region_end;
        }
    }
    // 计算覆盖0到max_physical_addr的位图大小
    uint32_t total_pages = (max_physical_addr + PAGE_SIZE - 1) / PAGE_SIZE;
    uint32_t bitmap_size = total_pages / 8;
    serial_printf("位图覆盖: 0x0 - %x\n", max_physical_addr);
    serial_printf("总页数: %d, 位图大小: %d 字节\n", total_pages, bitmap_size);

    // 初始化位图全为1
    memset(phys_bitmap, 0xFF, bitmap_size);

    // 根据内存映射标记各个区域
    for (uint32_t i = 0; i < mem_info.memory_map_count; i++)
    {
        uint32_t start_page = (uint32_t)mem_info.memory_map[i].addr / PAGE_SIZE;
        uint32_t end_page = ((uint32_t)mem_info.memory_map[i].addr +
                            (uint32_t)mem_info.memory_map[i].len) / PAGE_SIZE;
        
        if (start_page >= TOTAL_PAGES) continue;
        if (end_page > TOTAL_PAGES) end_page = TOTAL_PAGES;
        if (start_page >= end_page) continue;

        for(uint32_t page = start_page; page < end_page; page++)
        {
            if(mem_info.memory_map[i].type == 1)
                mark_page_free(page);
            else
                mark_page_used(page);
        }
    }


    for (uint32_t i = 0; i < mem_info.memory_map_count; i++)
{
    uint32_t start_addr = (uint32_t)mem_info.memory_map[i].addr;
    uint32_t end_addr = start_addr + (uint32_t)mem_info.memory_map[i].len;
    uint32_t start_page = start_addr / PAGE_SIZE;
    uint32_t end_page = end_addr / PAGE_SIZE;
    
    // 添加边界检查
    if (start_page >= total_pages) {
        serial_printf("跳过区域%d: 超出物理内存范围\n", i);
        continue;
    }
    if (end_page > total_pages) {
        end_page = total_pages;
    }
    if (start_page >= end_page) {
        serial_printf("跳过无效区域%d: 页面%d-%d\n", i, start_page, end_page);
        continue;
    }
    
    serial_printf("内存区域%d: 0x%x-0x%x, 类型:%d, 页面:%d-%d\n", 
                 i, start_addr, end_addr, 
                 mem_info.memory_map[i].type, start_page, end_page);
    
    // 检查前3个页面的标记情况
    for(uint32_t page = start_page; page < end_page && page < start_page + 3; page++)
    {
        bool old_state = is_page_free(page);
        
        if(mem_info.memory_map[i].type == 1) {
            mark_page_free(page);
            serial_printf("  页面%d: %s -> 空闲\n", page, old_state ? "空闲" : "已用");
        } else {
            mark_page_used(page);
            serial_printf("  页面%d: %s -> 已用\n", page, old_state ? "空闲" : "已用");
        }
    }
    
    if (end_page - start_page > 3) {
        serial_printf("  ... 还有%d个页面\n", end_page - start_page - 3);
    }
}
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

bool is_page_free(uint32_t page_index)
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
        kprintf(3, 0, "init_kernel_heap not alloc heap!");
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

    kprintf(3, 0, "---kernel_heap: 0x%x -> 0x%x (%d KB)---", 
       heap_phys, heap_phys + kheap.total_size, 
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