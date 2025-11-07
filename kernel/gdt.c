/* kernel/gdt.c */

#include <Hydrangea/gdt.h>

struct gdt_entry gdt_entries[3];

void init_gdt()
{
    gdt_entries[0].limit_low = 0;
    gdt_entries[0].base_low = 0;
    gdt_entries[0].base_middle = 0;
    gdt_entries[0].access = 0;
    gdt_entries[0].granularity = 0;
    gdt_entries[0].base_high = 0;

    gdt_entries[1].limit_low = 0xFFFF;
    gdt_entries[1].base_low = 0x0000;
    gdt_entries[1].base_middle = 0x00;
    gdt_entries[1].access = 0x9A;
    gdt_entries[1].granularity = 0xCF;
    gdt_entries[1].base_high = 0x00;
    
    gdt_entries[2].limit_low = 0xFFFF;
    gdt_entries[2].base_low = 0x0000;
    gdt_entries[2].base_middle = 0x00;
    gdt_entries[2].access = 0x92;
    gdt_entries[2].granularity = 0xCF;
    gdt_entries[2].base_high = 0x00;

    // 设置好gdt的指针
    struct gdt_ptr gdt;
    gdt.limit = sizeof(gdt_entries) - 1;
    gdt.base = (uint32_t)&gdt_entries;
    // 重载寄存器，刷新缓存
    asm volatile(
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
}