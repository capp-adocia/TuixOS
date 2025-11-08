/* kernel/page.c */

#include <Silan/page.h>
#include <Silan/serial.h>
#include <Silan/memory.h>

// 页目录项
uint32_t page_directory[1024] __attribute__((aligned(4096)));

// 第一个页表(映射前4MB)
uint32_t first_page_table[1024] __attribute__((aligned(4096)));

void init_page()
{
    // 1、初始化第一个页表
    for (uint32_t i = 0; i < PT_NUM; i++)
    {
        // 映射前4MB的物理页
        first_page_table[i] = (i << 12) | PRESENT_BIT | READ_WRITE_BIT;
    }
    // 2、页目录的第一个条目指向这个页表
    page_directory[0] = (uint32_t)first_page_table | PRESENT_BIT | READ_WRITE_BIT;
    
    // 3、其他条目设置不存在
    for (uint32_t i = 1; i < PDE_NUM; i++)
    {
        page_directory[i] = 0;
    }
    // 4、设置内核页保护
    setup_kernel_page_protection();

    // 5、设置CR3寄存器指向页目录
    enable_paging();
}

uint32_t calculate_page_count(uint32_t start_addr, uint32_t end_addr)
{
    uint32_t start_page = start_addr >> 12;
    uint32_t end_page = (end_addr + 0xFFF) >> 12;  // 向上取整
    return end_page - start_page;
}

void setup_kernel_page_protection()
{
    // 计算各段的页边界
    uint32_t text_start_page = (uint32_t)_kernel_text_start >> 12;
    uint32_t text_end_page = (uint32_t)_kernel_text_end >> 12;
    
    uint32_t rodata_start_page = (uint32_t)_kernel_rodata_start >> 12;
    uint32_t rodata_end_page = (uint32_t)_kernel_rodata_end >> 12;

    uint32_t data_start_page = (uint32_t)_kernel_data_start >> 12;
    uint32_t data_end_page = (uint32_t)_kernel_data_end >> 12;

    uint32_t bss_start_page = (uint32_t)_kernel_bss_start >> 12;
    uint32_t bss_end_page = (uint32_t)_kernel_bss_end >> 12;
    
    // 设置.text段为只读+可执行
    for (uint32_t i = text_start_page; i < text_end_page; i++)
    {
        first_page_table[i] = (i << 12) | PRESENT_BIT;
    }
    // 设置.rodata段为只读
    for (uint32_t i = rodata_start_page; i < rodata_end_page; i++)
    {
        first_page_table[i] = (i << 12) | PRESENT_BIT;
    }
    // 设置.data段为可读写
    for (uint32_t i = data_start_page; i < data_end_page; i++)
    {
        first_page_table[i] = (i << 12) | PRESENT_BIT | READ_WRITE_BIT;
    }
    // 设置.bss段为可读写
    for (uint32_t i = bss_start_page; i < bss_end_page; i++)
    {
        first_page_table[i] = (i << 12) | PRESENT_BIT | READ_WRITE_BIT;
    }
}

void enable_paging()
{
    uint32_t pd_physical = (uint32_t)page_directory;
    // 1、将CR3指向页目录物理地址
    __asm__ volatile("mov %0, %%cr3" : : "r"(pd_physical));
    // 2、设置CRO.PG位启动分页
    __asm__ volatile(
        "mov %%cr0, %%eax\n"
        "or $0x80000000, %%eax\n" // 设置PG位
        "mov %%eax, %%cr0"
        : : : "eax"
    );
}