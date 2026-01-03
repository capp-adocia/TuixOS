/* kernel/gdt.c */

#include <Tuix/sysconf.h>
#include <Tuix/gdt.h>
#include <Tuix/process.h>

void init_gdt()
{
    struct cpu* c = &(cpus[cpu_id]);
    
    c->gdt[0].limit_low = 0x0000;
    c->gdt[0].base_low = 0x0000;
    c->gdt[0].base_middle = 0x00;
    c->gdt[0].access = 0x00;
    c->gdt[0].granularity = 0x00;
    c->gdt[0].base_high = 0x00;

    c->gdt[KER_CI].limit_low = 0xFFFF;
    c->gdt[KER_CI].base_low = 0x0000;
    c->gdt[KER_CI].base_middle = 0x00;
    c->gdt[KER_CI].access = 0x9A;
    c->gdt[KER_CI].granularity = 0xCF;
    c->gdt[KER_CI].base_high = 0x00;

    c->gdt[KER_DI].limit_low = 0xFFFF;
    c->gdt[KER_DI].base_low = 0x0000;
    c->gdt[KER_DI].base_middle = 0x00;
    c->gdt[KER_DI].access = 0x92;
    c->gdt[KER_DI].granularity = 0xCF;
    c->gdt[KER_DI].base_high = 0x00;

    c->gdt[USR_CI].limit_low = 0xFFFF;
    c->gdt[USR_CI].base_low = 0x0000;
    c->gdt[USR_CI].base_middle = 0x00;
    c->gdt[USR_CI].access = 0xFA;
    c->gdt[USR_CI].granularity = 0xCF;
    c->gdt[USR_CI].base_high = 0x00;

    c->gdt[USR_DI].limit_low = 0xFFFF;
    c->gdt[USR_DI].base_low = 0x0000;
    c->gdt[USR_DI].base_middle = 0x00;
    c->gdt[USR_DI].access = 0xF2;
    c->gdt[USR_DI].granularity = 0xCF;
    c->gdt[USR_DI].base_high = 0x00;

    // 设置好gdt的指针
    struct gdt_ptr gdt;
    gdt.limit = sizeof(c->gdt) - 1;
    gdt.base = (uint32_t)&(c->gdt);
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
}

void set_tss_entry(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity)
{
    struct cpu* c = &(cpus[cpu_id]);
    c->gdt[num].base_low = (base & 0xFFFF);
    c->gdt[num].base_middle = (base >> 16) & 0xFF;
    c->gdt[num].base_high = (base >> 24) & 0xFF;
    c->gdt[num].limit_low = (limit & 0xFFFF);
    c->gdt[num].granularity = ((limit >> 16) & 0x0F) | (granularity & 0xF0);
    c->gdt[num].access = access;
}
