/* kernel/mm/vm.c */

#include <Tuix/sysconf.h>
#include <Tuix/panic.h>
#include <Tuix/serial.h>
#include <Tuix/mmu.h>
#include <stddef.h>
#include <Tuix/vm.h>
#include <Tuix/kalloc.h>
#include <string.h>
#include <Tuix/mulitiboot2.h>
#include <Tuix/gdt.h>
#include <Tuix/process.h>

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
        *pte = pa | perm | PTE_P; // 注意这里不要加入U位，用户不应当直接访问内核
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
        *pde = V2P(pgtab) | PTE_P | PTE_W | PTE_U;
        // 注意这里需要加入U位，否则到时进入用户进程无法访问用户进程代码的起始地址。
        // 还有这里参考xv6实现，实际上这个U位权限过于宽松了。
    }
    return &pgtab[PTX(va)];
}

void init_kvm(void)
{
    init_kmappings();
    kpgdir = setup_kvm(); // 创建kvm
    lcr3(V2P(kpgdir)); // 切换到内核页基址
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
    k_maps[2].phys_end = PHYSTOP;
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
    for(uint32_t i = 0; i < sizeof(k_maps) / sizeof(k_maps[0]);i++)
    {
        int erro = map_pages(pgdir, k_maps[i].virt, k_maps[i].phys_end - k_maps[i].phys_start,
                k_maps[i].phys_start, k_maps[i].perm);
        if(erro < 0)
        {
            PANIC("setup_kvm: map_pages映射出错");
            return 0;
        };
    }

    return pgdir;
}

void init_uvm(pde_t *pgdir, char *init, uint32_t size)
{
    char* mem;
    if(size > PGSIZE)
        PANIC("init_uvm: more than a page");
    mem = kalloc();
    memset(mem, 0, PGSIZE);
    // 将虚拟地址0映射到这一页映射
    map_pages(pgdir, 0, PGSIZE, V2P(mem), PTE_W | PTE_U);
    memmove(mem, init, size); // 将init所在的数据移动到分配到的这页上来
}

void switch_uvm(struct proc* p)
{
    struct cpu* cur_cpu = &(cpus[cpu_id]);
    if(p == 0)
        PANIC("switch_uvm: no process");
    if(p->kstack == 0)
        PANIC("switch_uvm: no kstack");
    if(p->pgdir == 0)
        PANIC("switch_uvm: no pgdir");

    set_tss_entry(TSS_I, (uint32_t)&(cur_cpu->ts), sizeof(struct tss_entry) - 1, 0x89, 0x40);

    cur_cpu->ts.ss0 = KER_DS;
    cur_cpu->ts.esp0 = (uint32_t)p->kstack + KSTACK_SIZE;
    cur_cpu->ts.iomap_base = (uint16_t)0xFFFF;

    /* 设置tr寄存器指向tss位置0x28是tss的偏移位置 */
    __asm__ volatile("ltr %%ax" : : "a" (TSS_S));

    // 这里需要切换到进程的地址空间
    lcr3(V2P(p->pgdir));
}

void switch_kvm(void)
{
    lcr3(V2P(kpgdir));
}

