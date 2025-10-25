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
    kprintf(0, 0, "%s", "HydrangeaOS v0.01");
    // // 内存初始化
    // init_physical_memory();
    // uint32_t total, free;
    // get_memory_info(&total, &free);
    // kprintf(2, 0, "Total: %d KB", total);
    // kprintf(3, 0, "Free: %d KB", free);

    // init_kernel_heap();

    // // 基础检查
    // int page_index = 259;
    // if (page_is_free(page_index)) {
    //     kprintf(4, 0, "Page %d is free", page_index);
    // } else {
    //     kprintf(4, 0, "Page %d is used", page_index);
    // }
    // test_malloc();
}

void kernel_main(void)
{
    // 基础显示
    init();
    __asm__ volatile("mov $0xFF, %al\nout %al, $0x21\nout %al, $0xA1");
    init_idt();
    init_pic();
    // 开启中断
    __asm__ volatile("sti");

    __asm__ volatile(
        "mov $1, %eax\n\t" // EAX = 1
        "mov $0, %ebx\n\t" // EBX = 0
        "div %ebx\n\t"     // EAX / EBX → 触发除零异常
    );

    while(1){kprint(24, 0, "In loop");}
}