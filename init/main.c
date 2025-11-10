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
#include <Silan/timer.h>
#include <Silan/page.h>
#include <Silan/serial.h>
#include <Silan/mulitiboot2.h>
#include <Silan/process.h>

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
    enable_irq(IRQ_KEYBOARD); // 启用键盘
    // 3. 全局启用中断
    __asm__ volatile("sti");
    kprint(11, 0, "Keyboard system ready...");
}

void init_timer_system(void)
{
    enable_irq(IRQ_TIMER);
    __asm__ volatile("sti");
    kprint(12, 0, "timer system ready...");
}

void kernel_main(uint32_t magic, uint32_t mbi_addr)
{
    __asm__ volatile("cli");
    /* 初始化串口 */
    init_serial();
    serial_printf("=== Silan OS 启动 ===\r\n");
    /* 基础显示 */
    init_show();
    /* 解析mbi_addr */
    parse_multiboot2_info(magic, mbi_addr);
    /* 设置好gdt表项 */
    init_gdt();
    /* 内存初始化 */
    init_physical_memory();
    /* 初始化堆，必须在内核初始化后做 */
    init_kernel_heap();
    /* 初始化pic和idt表 */
    init_pic();
    init_idt();
    /* 初始化定时器 */
    init_timer(100);
    /* 启用分页 */
    init_page();
    /* 开启中断 */
    __asm__ volatile("sti");
    /* TEST */
    init_keyboard_system();
    // init_timer_system();


    init_task();

    serial_printf("\n任务结束，已经返回内核\n");

    while(1) __asm__ volatile("hlt");
}