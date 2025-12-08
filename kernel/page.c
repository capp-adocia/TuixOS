/* kernel/page.c */

#include <Tuix/page.h>
#include <Tuix/serial.h>
#include <Tuix/memory.h>

uint32_t page_directory[1024] __attribute__((aligned(4096)));

void init_page(void)
{
    /*
        实现恒等映射的几个步骤：
        1、首先为页目录中的1024个页表创建应有的物理页，然后将对应的页目录项的内容设置为对应页表的起始物理地址 + 标志位
        2、循环为每一个页表的1024个页表项写入虚拟地址对应的物理地址，将对应的页表项的内容设置为虚拟地址对应的物理地址 + 标志位
        3、注意第2步写入的物理地址的计算方式，是按照实际虚拟转物理得到的地址，比如页目录对应1，页表索引对应3，那么就是(1 * 1024 + 3) * 4096，就是1号页表的3号页表项
    */
    serial_printf("[Page] Simple identity mapping\n");
    
    for (uint32_t pd_index = 0; pd_index < PDE_NUM; pd_index++)
    {
        uint32_t* new_pt = create_page_table(pd_index);
        
        for (int pt_index = 0; pt_index < PTE_NUM; pt_index++)
        {
            // 简单的：虚拟地址 = 物理地址
            uint32_t virtual_addr = (pd_index << 22) | (pt_index << 12);
            uint32_t physical_addr = virtual_addr;
            
            // 所有页面用户可访问（调试）
            uint32_t flags = PRESENT_BIT | USER_BIT | READ_WRITE_BIT;
            
            new_pt[pt_index] = physical_addr | flags;
        }
    }
    // 设置内核页保护
    setup_kernel_page_protection();
    
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
    page_directory[pd_index] = (uint32_t)new_pt | PRESENT_BIT;
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

void setup_kernel_page_protection(void)
{
    uint32_t* first_pt = get_page_table(0x0);
    
    // 只有.data和.bss段设置为可读写
    for (uint32_t page_index = get_pt_index(mem_info.kdata_start_addr);
         page_index < (mem_info.kdata_end_addr + 0xFFF) >> 12; page_index++)
        first_pt[page_index] |= READ_WRITE_BIT;

    for (uint32_t page_index = get_pt_index(mem_info.kbss_start_addr);
         page_index < (mem_info.kbss_end_addr + 0xFFF) >> 12; page_index++)
        first_pt[page_index] |= READ_WRITE_BIT;
}

void enable_paging(void)
{
    uint32_t pd_physical = (uint32_t)page_directory;
    // 将CR3指向页目录物理地址
    __asm__ volatile("mov %0, %%cr3" : : "r"(pd_physical));
    // 设置CRO.PG位启动分页
    __asm__ volatile(
        "mov %%cr0, %%eax\n"
        "or $0x80000000, %%eax\n" // 设置PG位
        "mov %%eax, %%cr0\n"
        : : : "eax"
    );
    // 刷新TLB
    __asm__ volatile("mov %%cr3, %%eax; mov %%eax, %%cr3" ::: "eax");
    // serial_printf("\n=== Verifying Critical Mappings ===\n");
    //
    // // 重要地址列表
    // struct {
    //     uint32_t vaddr;
    //     const char* name;
    //     uint32_t expected_phys;
    // } checks[] = {
    //     {0x10020, ".text start", 0x10020},
    //     {0x140eb, "kernel_main", 0x140eb},
    //     {0x17e86, "task_A", 0x17e86},
    //     {0x17000, "task_A page", 0x17000},
    // };
    //
    // for (int i = 0; i < 4; i++) {
    //     uint32_t vaddr = checks[i].vaddr;
    //     uint32_t page = vaddr & 0xFFFFF000;
    //     uint32_t pd_idx = page >> 22;
    //     uint32_t pt_idx = (page >> 12) & 0x3FF;
    //
    //     uint32_t* page_dir = (uint32_t*)page_directory;
    //     uint32_t pde = page_dir[pd_idx];
    //
    //     serial_printf("\n%s (0x%x):\n", checks[i].name, vaddr);
    //
    //     if (pde & 0x1) {
    //         uint32_t* page_table = (uint32_t*)(pde & 0xFFFFF000);
    //         uint32_t pte = page_table[pt_idx];
    //         uint32_t mapped_phys = pte & 0xFFFFF000;
    //
    //         serial_printf("  Maps to: 0x%x\n", mapped_phys);
    //         serial_printf("  Expected: 0x%x\n", checks[i].expected_phys & 0xFFFFF000);
    //         serial_printf("  PTE: 0x%x (P=%d, U/S=%d)\n", 
    //                      pte, (pte>>0)&1, (pte>>2)&1);
    //
    //         if (mapped_phys == (checks[i].expected_phys & 0xFFFFF000)) {
    //             serial_printf("  ✓ Mapping correct\n");
    //         } else {
    //             serial_printf("  ✗ Mapping wrong!\n");
    //         }
    //     } else {
    //         serial_printf("  ✗ Page table not present!\n");
    //     }
    // }
    
    // uint32_t mapped_pages = 0;
    // uint32_t max_mapped_addr = 0;
    //
    // for (uint32_t i = 0; i < 1024; i++)
    // {
    //     if (page_directory[i] & PRESENT_BIT)
    //     {
    //         // 这个页目录项有效，映射了4MB区域
    //         uint32_t region_start = i * 4 * 1024 * 1024;  // 每个PDE映射4MB
    //         uint32_t region_end = region_start + 4 * 1024 * 1024 - 1;
    //
    //         serial_printf("PDE[%d]: 映射 0x%x - 0x%x\n", i, region_start, region_end);
    //
    //         mapped_pages += 1024;  // 每个页表有1024个页面
    //         max_mapped_addr = region_end;
    //     }
    // }
    //
    // serial_printf("总计: %d 页 (%d MB)\n", mapped_pages, mapped_pages * 4 / 1024);
    // serial_printf("最大映射地址: 0x%x\n", max_mapped_addr);
}

void unmap_page(uint32_t virtual_addr)
{
    uint32_t pd_index = get_pd_index(virtual_addr);
    uint32_t pt_index = get_pt_index(virtual_addr);
    uint32_t* pt = get_page_table(virtual_addr);
    
    if (pt)
    {
        pt[pt_index] = 0;  // 清除存在位
        serial_printf("已取消映射 %x\n", virtual_addr);
    }
}

void check_mapping(uint32_t virtual_addr)
{
    uint32_t pd_index = get_pd_index(virtual_addr);
    uint32_t pt_index = get_pt_index(virtual_addr);
    
    serial_printf("检查地址 %x:\n", virtual_addr);
    serial_printf("  页目录索引: %x\n", pd_index);
    serial_printf("  页表索引: %x\n", pt_index);
    
    uint32_t* pt = get_page_table(virtual_addr);
    if (pt)
    {
        uint32_t pte = pt[pt_index];
        serial_printf("  页表项: %x\n", pte);
        serial_printf("  映射到物理地址: %x\n", pte & 0xFFFFF000);
        serial_printf("  存在位: %s\n", (pte & PRESENT_BIT) ? "是" : "否");
    } 
    else serial_printf("  页表不存在\n");
}
