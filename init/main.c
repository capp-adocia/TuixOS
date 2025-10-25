/* init/main.c 实现C内核 */
#include <Hydrangea/screen.h>
#include <Hydrangea/memory.h>
#include <string.h>
#include <stddef.h>
#include <def.h>
#include <Hydrangea/idt.h>
#include <Hydrangea/pic.h>

// 1. 先声明所有函数
void kernel_main(void);
void init(void);
void print_LOGO(void);
void test_malloc(void);

// 2. 常量定义
const char *logo[] = {
    "H   H  Y   Y  DDDD   RRRRR     A    N   N  GGGG  EEEEEE    A  ",
    "H   H   Y Y   D   D  R    R   A A   NN  N G      E        A A ",
    "HHHHH    Y    D   D  RRRRRR  AAAAA  N N N G  GGG EEEEE   AAAAA",
    "H   H    Y    D   D  R  R   A     A N  NN G   GG E      A     A",
    "H   H    Y    DDDD   R  RRR A     A N   N  GGGG  EEEEEE A     A"};

// 3. 入口点
__attribute__((naked)) void _start(void)
{
    asm volatile(
        "call kernel_main\n"
        "hlt\n"
        "jmp .\n");
}

// 4. 辅助函数
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

// 5. 测试函数集中放在一起
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

void kernel_main(void)
{
    // 基础显示
    init();
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
        asm volatile("hlt");
    }
}