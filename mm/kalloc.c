/* mm/kalloc.c */

#include <Tuix/kalloc.h>
#include <Tuix/mmu.h>
#include <Tuix/screen.h>
#include <string.h>
#include <def.h>
#include <Tuix/panic.h>

/* 空闲列表 */
struct run
{
    struct run* next;
};
struct
{
    // 后面需要加入锁
    struct run* freelist;
} kmem;

void init_physical_memory(void* vstart, void* vend)
{
    free_range(vstart, vend);
}

void free_range(void* vstart, void* vend)
{
    char* p = (char*)PGROUNDUP((uint32_t)vstart);
    for(;p + PGSIZE <= (char*)vend; p += PGSIZE)
        kfree(p);
}

char* kalloc(void)
{
    // 从空闲页中取出一页
    struct run* r = kmem.freelist;
    if(r) kmem.freelist = kmem.freelist->next;
    return (char*)r;
}

void kfree(char* ptr)
{
    struct run* r;
    // 填充特殊数值0xdeadbeef
    uint32_t* p = (uint32_t*)ptr;
    for (int i = 0; i < PGSIZE / MMU_SIZE; i++)
        p[i] = FREE_STR;

    r = (struct run*)ptr;
    r->next = kmem.freelist;
    kmem.freelist = r;
}
