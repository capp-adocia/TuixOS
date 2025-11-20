/* kernel/gdt.c */

#include <TuiPlus/gdt.h>

struct gdt_entry gdt_entries[6];

void init_gdt()
{
    gdt_entries[0].limit_low = 0;
    gdt_entries[0].base_low = 0;
    gdt_entries[0].base_middle = 0;
    gdt_entries[0].access = 0;
    gdt_entries[0].granularity = 0;
    gdt_entries[0].base_high = 0;

    gdt_entries[1].limit_low = 0xFFFF;      // 内核代码段 (0x08)
    gdt_entries[1].base_low = 0x0000;
    gdt_entries[1].base_middle = 0x00;
    gdt_entries[1].access = 0x9A;
    gdt_entries[1].granularity = 0xCF;
    gdt_entries[1].base_high = 0x00;
    
    gdt_entries[2].limit_low = 0xFFFF;      // 内核数据段 (0x10)
    gdt_entries[2].base_low = 0x0000;
    gdt_entries[2].base_middle = 0x00;
    gdt_entries[2].access = 0x92;
    gdt_entries[2].granularity = 0xCF;
    gdt_entries[2].base_high = 0x00;

    // 添加用户态段
    gdt_entries[3].limit_low = 0xFFFF;      // 用户代码段 (0x18)
    gdt_entries[3].base_low = 0x0000;
    gdt_entries[3].base_middle = 0x00;
    gdt_entries[3].access = 0xFA;           // DPL=3
    gdt_entries[3].granularity = 0xCF;
    gdt_entries[3].base_high = 0x00;

    gdt_entries[4].limit_low = 0xFFFF;      // 用户数据段 (0x20)
    gdt_entries[4].base_low = 0x0000;
    gdt_entries[4].base_middle = 0x00;
    gdt_entries[4].access = 0xF2;           // DPL=3
    gdt_entries[4].granularity = 0xCF;
    gdt_entries[4].base_high = 0x00;

    set_tss_entry(5, (uint32_t)&tss, sizeof(struct tss_entry)-1, 0x89, 0x40);
    
    // 设置好gdt的指针
    struct gdt_ptr gdt;
    gdt.limit = sizeof(gdt_entries) - 1;
    gdt.base = (uint32_t)&gdt_entries;
    // 重载寄存器，刷新缓存
    __asm__ volatile(
        "lgdt %0\n"
        "ljmp $0x08, $reload_cs\n"
        "reload_cs:\n"
        "mov $0x10, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "mov %%ax, %%ss\n"
        : :"m"(gdt)
    );
    
    /* 加载tss */
    __asm__ volatile("ltr %%ax" : : "a" (0x28));
}

void set_tss_entry(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity)
{
    gdt_entries[num].base_low = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high = (base >> 24) & 0xFF;
    
    gdt_entries[num].limit_low = (limit & 0xFFFF);
    gdt_entries[num].granularity = ((limit >> 16) & 0x0F) | (granularity & 0xF0);
    
    gdt_entries[num].access = access;
}