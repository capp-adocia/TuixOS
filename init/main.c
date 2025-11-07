/* init/main.c 实现C内核 */
#include <Silan/screen.h>
#include <Silan/memory.h>
#include <string.h>
#include <stddef.h>
#include <def.h>
#include <Silan/gdt.h>
#include <Silan/idt.h>
#include <Silan/pic.h>
#include <Silan/serial.h>

void kernel_main(uint32_t magic, uint32_t mbi_addr);
void init_show(void);
void print_LOGO(void);

const char *logo[] = {
    "SSSSSS  IIIIII  LL    A     NN   NN ",
    "SS        II    LL   A A    NNN  NN ",
    " SSSSS    II    LL  AAAAA   NN N NN ",
    "    SS    II    LL AA   AA  NN  NNN ",
    "SSSSSS  IIIIII  LL AA   AA  NN   NN "};

void print_LOGO(void)
{
    int logo_height = sizeof(logo) / sizeof(logo[0]);
    for (int i = 0; i < logo_height; i++)
    {
        kprint(9 + i, 21, logo[i]);
    }
    kprint(20, 33, "SilanOS v0.01");

    volatile int c = 500000; // 加 volatile 避免被优化
    while (c--)
    {
        kprint(0, 0, "LOGO!");
    }
    kprint(0, 0, "Done!");
}

void init_show(void)
{
    clear_screen();
    // print_LOGO();
    // clear_screen();
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

void kernel_main(uint32_t magic, uint32_t mbi_addr)
{
    asm volatile("cli");
    /* 基础显示 */
    init_show();
    /* 设置好gdt表项 */
    init_gdt();
    /* 初始化串口 */
    init_serial();
    serial_printf("=== Silan OS 启动 ===\r\n");

    /* 内存初始化 */
    init_physical_memory();
    /* 初始化堆，必须在内核初始化后做 */
    init_kernel_heap();
    /* 初始化pic和idt表 */
    init_pic();
    init_idt();
    /* 开启中断 */
    asm volatile("sti");
    
    /* TEST */
    init_keyboard_system();
    kprint(12, 0, "System ready. Keyboard should work now.");


    while(1) asm volatile("hlt");
}