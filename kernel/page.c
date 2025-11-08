/* kernel/page.c */

#include <Silan/page.h>
#include <Silan/serial.h>
#include <Silan/memory.h>

uint32_t page_directory[1024] __attribute__((aligned(4096)));

void init_page()
{
    // 创建页表
    for (uint32_t i = 0; i < PDE_NUM; i++) create_page_table(i);

    // 设置内核页保护
    // setup_kernel_page_protection();

    // 设置CR3寄存器指向页目录
    enable_paging();
}

uint32_t* create_page_table(uint32_t pd_index)
{
    int error_code;
    uint32_t* new_pt = alloc_page(&error_code);
    if (error_code < 0)
    {
        serial_printf("创建页表失败: pd_index=%d, error=%d\n", pd_index, error_code);
        return NULL;
    }
    
    // 建立恒等映射
    for (int i = 0; i < 1024; i++)
    {
        uint32_t physical_addr = (pd_index * 1024 + i) * PAGE_SIZE;
        new_pt[i] = physical_addr | PRESENT_BIT | READ_WRITE_BIT;
    }
    
    page_directory[pd_index] = (uint32_t)new_pt | PRESENT_BIT | READ_WRITE_BIT;
    
    return new_pt;
}

uint32_t* get_page_table(uint32_t virtual_addr)
{
    // 页目录号
    uint32_t pd_index = get_pd_index(virtual_addr);
    uint32_t pde = page_directory[pd_index]; // 页目录项的内容存储了页表索引号
    if(!(pde & PRESENT_BIT))
    {
        serial_printf("页表不存在\n");
        return NULL;
    }
    uint32_t pt_phys = pde & 0xFFFFF000; // 提取页表物理地址
    return (uint32_t*)pt_phys;
}

void map_page(uint32_t virtual_addr, uint32_t physical_addr, uint32_t flags)
{
    uint32_t pd_index = get_pd_index(virtual_addr);
    uint32_t pt_index = get_pt_index(virtual_addr);
    uint32_t* page_table = get_page_table(virtual_addr);
    if(!page_table) page_table = create_page_table(pd_index);
    page_table[pt_index] = physical_addr | flags;
}

uint32_t calculate_page_count(uint32_t start_addr, uint32_t end_addr)
{
    uint32_t start_page = start_addr >> 12;
    uint32_t end_page = (end_addr + 0xFFF) >> 12; // 向上取整
    return end_page - start_page;
}

void setup_kernel_page_protection()
{
    // 需要通过页目录访问第一个页表来设置保护
    uint32_t* first_pt = get_page_table(0x0);
    // 设置.text段为只读+可执行
    for (uint32_t i = mem_info.ktext_start_addr; i < mem_info.ktext_end_addr; i++)
        first_pt[i] = (i << 12) | PRESENT_BIT;
    // 设置.rodata段为只读
    for (uint32_t i = mem_info.krodata_addr; i < mem_info.krodata_end_addr; i++)
        first_pt[i] = (i << 12) | PRESENT_BIT;
    // 设置.data段为可读写
    for (uint32_t i = mem_info.kdata_start_addr; i < mem_info.kdata_end_addr; i++)
        first_pt[i] = (i << 12) | PRESENT_BIT | READ_WRITE_BIT;
    // 设置.bss段为可读写
    for (uint32_t i = mem_info.kbss_start_addr; i < mem_info.kbss_end_addr; i++)
        first_pt[i] = (i << 12) | PRESENT_BIT | READ_WRITE_BIT;
}

void enable_paging()
{
    uint32_t pd_physical = (uint32_t)page_directory;
    // 将CR3指向页目录物理地址
    __asm__ volatile("mov %0, %%cr3" : : "r"(pd_physical));
    // 设置CRO.PG位启动分页
    __asm__ volatile(
        "mov %%cr0, %%eax\n"
        "or $0x80000000, %%eax\n" // 设置PG位
        "mov %%eax, %%cr0"
        : : : "eax"
    );
}