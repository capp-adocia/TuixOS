/* kernel/vm.c */

#include <Tuix/panic.h>
#include <Tuix/serial.h>
#include <Tuix/mmu.h>
#include <stddef.h>
#include <Tuix/vm.h>
#include <Tuix/kalloc.h>
#include <string.h>
#include <Tuix/mulitiboot2.h>

pde_t* kpgdir;

struct kmap k_maps[4];

static int map_pages(pde_t* pgdir, void* va, uint32_t size, uint32_t pa, int perm)
{
    char *start, *last;
    pte_t *pte;

    start = (char*)PGROUNDDOWN((uint32_t)va);
    last = (char*)PGROUNDDOWN(((uint32_t)va) + size - 1);
    
    // 遍历指定的范围
    while(true)
    {
        if((pte = walk_pgdir(pgdir, start, 1)) == 0)
            return -1;
        if(*pte & PTE_P)
            PANIC("映射到重复的空间了!");
        *pte = pa | perm | PTE_P;
        if(start == last)
            break;
        // 虚拟地址和物理地址要同步更新
        start += PGSIZE;
        pa += PGSIZE;
    }
    return 0;
}

pte_t* walk_pgdir(pde_t* pgdir, const void* va, int alloc)
{
    pde_t *pde;      // 指向页目录项的指针
    pte_t *pgtab;    // 指向页表的指针
    // 获取页目录项
    pde = &pgdir[PDX(va)];
    // 检查页表是否存在
    if(*pde & PTE_P)
    {
        // 从页目录项中提取页表的物理地址，转换为虚拟地址
        pgtab = (pte_t*)P2V(PTE_ADDR(*pde));
    }
    else
    {
        // 页表不存在，可能需要分配
        if(!alloc || (pgtab = (pte_t*)kalloc()) == 0)
            return 0;  // 不分配或分配失败
        // 清空新分配的页表（所有 PTE 初始为0）
        memset(pgtab, 0, PGSIZE);
        // 设置页目录项，指向新页表
        // V2P(pgtab): 将页表虚拟地址转为物理地址
        *pde = V2P(pgtab) | PTE_P | PTE_W;
    }
    return &pgtab[PTX(va)];
}

void init_kvm(void)
{
    init_kmappings();
    kpgdir = setup_kvm(); // 创建kvm
    asm volatile("movl %0,%%cr3" : : "r" (V2P(kpgdir)));

    // 检查 0x80107cce 的 PTE
    volatile uint32_t va = 0x80107cce;
    volatile pde_t* pde = &kpgdir[PDX(va)];

    if((*pde) & PTE_P)
    {
        volatile pte_t* pgtab = (pte_t*)P2V(PTE_ADDR(*pde));
        volatile pte_t* pte = &pgtab[PTX(va)];
    }

}

void init_kmappings(void)
{
    // I/O 空间
    k_maps[0].virt = (void*)KERNBASE;
    k_maps[0].phys_start = 0;
    k_maps[0].phys_end = EXTMEM;
    k_maps[0].perm = PTE_W;
    
    // 内核代码+只读数据
    k_maps[1].virt = (void*)KERNLINK;
    k_maps[1].phys_start = V2P(KERNLINK);
    k_maps[1].phys_end = V2P(mem_info.kdata_start_addr);
    k_maps[1].perm = 0;
    
    // 内核数据+内存
    k_maps[2].virt = (void*)mem_info.kdata_start_addr;
    k_maps[2].phys_start = V2P(mem_info.kdata_start_addr);
    // TODO:后续需要动态获取到物理内存的最后
    k_maps[2].phys_end = 0xE000000;
    k_maps[2].perm = PTE_W;
    
    // 设备空间
    k_maps[3].virt = (void*)DEVSPACE;
    k_maps[3].phys_start = DEVSPACE;
    k_maps[3].phys_end = 0;  // 这里需要为0
    k_maps[3].perm = PTE_W;
}

pde_t* setup_kvm(void)
{
    // 从空闲页框中分配出一页
    pde_t* pgdir = (pde_t*)kalloc();
    if(pgdir == 0) return NULL;

    memset(pgdir, 0, PGSIZE); // 清空这一页

    // 遍历整个kmap并为它设置好地址+标志位
    for(int i = 0; i < sizeof(k_maps) / sizeof(k_maps[0]);i++)
    {
        int erro = map_pages(pgdir, k_maps[i].virt, k_maps[i].phys_end - k_maps[i].phys_start,
                k_maps[i].phys_start, k_maps[i].perm);
        // serial_printf("k_maps[%d],物理起始地址：%x, 物理结束地址:%x \n", i, k_maps[i].phys_start, k_maps[i].phys_end);
        if(erro < 0)
        {
            PANIC("setup_kvm: map_pages映射出错");
            return 0;
        };
    }

    return pgdir;
}
