/* init/main.c 实现C内核 */
#include <Tuix/gdt.h>
#include <Tuix/idt.h>
#include <Tuix/kalloc.h>
#include <Tuix/mmu.h>
#include <Tuix/vm.h>
#include <Tuix/mulitiboot2.h>
#include <Tuix/pic.h>
#include <Tuix/process.h>
#include <Tuix/screen.h>
#include <Tuix/serial.h>
#include <Tuix/timer.h>
#include <def.h>
#include <Tuix/panic.h>

void kernel_main(uint32_t magic, uint32_t mbi_addr);
void init_show(void);
void print_LOGO(void);

const char *logo[] = {"TTTTTTTT  UU   UU  IIIIII  +++   +++",
                      "   TT     UU   UU    II       +++   ",
                      "   TT     UU   UU    II       +++   ",
                      "   TT     UU   UU    II       +++   ",
                      "   TT      UUUUU   IIIIII  +++   +++"};

void print_LOGO(void)
{
    int logo_height = sizeof(logo) / sizeof(logo[0]);
    for (int i = 0; i < logo_height; i++)
        kprint(9 + i, 21, logo[i]);
    
    kprint(20, 33, "TuixOS v0.01");

    volatile int c = 500000; // 加 volatile 避免被优化
    while (c--)
        kprint(0, 0, "LOGO!");
    
    kprint(0, 0, "Done!");
}

void init_show(void) 
{
    clear_screen();
    print_LOGO();
    // clear_screen();
}

void init_keyboard_system(void)
{
    // 1. 设置IDT中的键盘中断门
    // 2. 启用键盘IRQ
    enable_irq(IRQ_KEYBOARD); // 启用键盘
    // 3. 全局启用中断
    __asm__ volatile("sti");
    kprint(5, 0, "Keyboard system ready...");
}

void kernel_main(uint32_t magic, uint32_t mbi_addr)
{
    /* 初始化串口 */
    init_serial();
    /* 基础显示 */
    init_show();
    /* 解析mbi_addr */
    parse_multiboot2_info(magic, mbi_addr);
    /* 设置好gdt表项 */
    init_gdt();
    /* 为存储页表先初始化一部分内存 */
    init_physical_memory((uint32_t*)mem_info.kernel_end_addr, P2V(4*1024*1024));
    serial_printf("初始化一部分空间后，空闲列表大小为：%dKB\n", kmem_size() / 1024);
    /* 分配内核页表 */
    init_kvm();
    serial_printf("分配页表空间后，空闲列表大小为：%dKB\n", kmem_size() / 1024);
    /* 分配剩余的空闲页 */
    init_physical_memory(P2V(4*1024*1024), P2V(PHYSTOP));
    serial_printf("回收剩余空间后，空闲列表大小为：%dKB\n", kmem_size() / 1024);
    /* 初始化pic和idt表 */
    init_pic();
    init_idt();
    /* 初始化定时器 */
    init_timer(20);
    /* 启用键盘中断 */
    init_keyboard_system();
    /* 初始化第一个用户进程 */
    init_user();
    /* 执行调度以启动第一个用户进程 */
    launch_first_proc(); // 调度器执行一次调度
    /* 不会执行这里，因为已经开始调度了 */
    while (1) { __asm__ volatile("hlt"); }
}
