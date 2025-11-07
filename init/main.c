/* init/main.c 实现C内核 */
#include <Hydrangea/screen.h>
#include <Hydrangea/memory.h>
#include <string.h>
#include <stddef.h>
#include <def.h>
#include <Hydrangea/idt.h>
#include <Hydrangea/pic.h>

void kernel_main(unsigned int magic, unsigned int addr);
void init(void);
void print_LOGO(void);
void test_malloc(void);

const char *logo[] = {
    "H   H  Y   Y  DDDD   RRRRR     A    N   N  GGGG  EEEEEE    A  ",
    "H   H   Y Y   D   D  R    R   A A   NN  N G      E        A A ",
    "HHHHH    Y    D   D  RRRRRR  AAAAA  N N N G  GGG EEEEE   AAAAA",
    "H   H    Y    D   D  R  R   A     A N  NN G   GG E      A     A",
    "H   H    Y    DDDD   R  RRR A     A N   N  GGGG  EEEEEE A     A"};


void print_LOGO(void)
{
    int logo_height = sizeof(logo) / sizeof(logo[0]);
    for (int i = 0; i < logo_height; i++)
    {
        kprint(logo[i], 9 + i, 9);
    }
    kprint(20, 30, "HydrangeaOS v0.01");

    volatile int c = 500000; // 加 volatile 避免被优化
    while (c--)
    {
        kprint(0, 0, "LOGO!");
    }
    kprint(0, 0, "Done!");
}

void test_malloc(void)
{
}

void init(void)
{
    clear_screen();
    kprintf(0, 25, "%s", "HydrangeaOS v0.01");
}

void init_keyboard_system(void)
{
    // 1. 设置IDT中的键盘中断门
    // 2. 启用键盘IRQ
    enable_irq(1); // 启用键盘
    // 3. 全局启用中断
    asm volatile("sti");
    
    kprint(11, 0, "Keyboard system ready. Start typing...");
}
struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));
struct gdt_entry gdt_entries[3];
void setup_gdt() {
    // 空描述符 (0x00)
    gdt_entries[0].limit_low = 0;
    gdt_entries[0].base_low = 0;
    gdt_entries[0].base_middle = 0;
    gdt_entries[0].access = 0;
    gdt_entries[0].granularity = 0;
    gdt_entries[0].base_high = 0;
    
    // 代码段 (0x08)
    gdt_entries[1].limit_low = 0xFFFF;
    gdt_entries[1].base_low = 0;
    gdt_entries[1].base_middle = 0;
    gdt_entries[1].access = 0x9A;        // 存在，环0，代码段，可读
    gdt_entries[1].granularity = 0xCF;   // 4K粒度，32位模式
    gdt_entries[1].base_high = 0;
    
    // 数据段 (0x10)
    gdt_entries[2].limit_low = 0xFFFF;
    gdt_entries[2].base_low = 0;
    gdt_entries[2].base_middle = 0;
    gdt_entries[2].access = 0x92;        // 存在，环0，数据段，可写
    gdt_entries[2].granularity = 0xCF;   // 4K粒度，32位模式
    gdt_entries[2].base_high = 0;
}
void kernel_main(uint32_t magic, uint32_t mbi_addr)
{
    // 基础显示
    init();
    setup_gdt();
    struct gdt_ptr old_gdt;
    asm volatile ("sgdt %0" : "=m"(old_gdt));
    kprintf(16, 0, "GRUB GDT: base=0x%x, limit=0x%x", old_gdt.base, old_gdt.limit);
    struct gdt_ptr new_gdt_ptr;
    new_gdt_ptr.limit = sizeof(gdt_entries) - 1;
    new_gdt_ptr.base = (uint32_t)&gdt_entries;

    asm volatile("lgdt %0" : : "m"(new_gdt_ptr));

    asm volatile(
        "ljmp $0x08, $1f\n"
        "1:\n"
        "mov $0x10, %ax\n"
        "mov %ax, %ds\n"
        "mov %ax, %es\n"
        "mov %ax, %fs\n"
        "mov %ax, %gs\n"
        "mov %ax, %ss\n"
    );
    
    // 内存初始化
    init_physical_memory();
    // 初始化堆,必须在内核初始化后做
    init_kernel_heap();
    // 初始化pic和idt表
    init_pic();
    init_idt();
    // 开启中断
    __asm__ volatile("sti");
    
    init_keyboard_system();

    kprint(12, 0, "System ready. Keyboard should work now.");

    while(1){
        // asm volatile("hlt");
    }
}