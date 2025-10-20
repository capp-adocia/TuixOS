// init/main.c 实现C内核
#include <string.h>
#include <screen.h>
#include <memory.h>

void _start(void);
void kernel_main(void);

void _start(void)
{
    kernel_main();
}

const char* logo[] = {
    "H   H  Y   Y  DDDD   RRRRR     A    N   N  GGGG  EEEEEE    A  ",
    "H   H   Y Y   D   D  R    R   A A   NN  N G      E        A A ",
    "HHHHH    Y    D   D  RRRRRR  AAAAA  N N N G  GGG EEEEE   AAAAA",
    "H   H    Y    D   D  R  R   A     A N  NN G   GG E      A     A",
    "H   H    Y    DDDD   R  RRR A     A N   N  GGGG  EEEEEE A     A"
};

void print_LOGO(void)
{
    int logo_height = sizeof(logo) / sizeof(logo[0]);
    // 显示logo
    for (int i = 0; i < logo_height; i++) {
        kprint(logo[i], 9 + i, 9);
    }
    // 显示固定版本信息
    kprint("HydrangeaOS v0.01", 20, 30);
    
    int c = 500000;
    while(c--) {
        kprint("LOGO!", 0, 0);
    }

    kprint("Done!", 0, 0);
}
#define PAGE_SIZE 4096
#define TOTAL_MEMORY 16 * 1024 * 1024
#define TOTAL_PAGES (TOTAL_MEMORY / PAGE_SIZE)


/* 位图数组 */
uint8_t phys_bitmap[TOTAL_PAGES / 8]; // 分配4096

void init_physical_memory(void)
{
    uint32_t memory_end = 16 * 1024 * 1024;
    uint32_t used_end = 1 * 1024 * 1024; // 内核结束的位置

    // 1. 探测内存大小（最简单：先假设有16MB）
    memset(phys_bitmap, 0 , sizeof(phys_bitmap));
    // 2. 初始化位图：大部分标记为空闲

    // 3. 标记已使用的区域（内核代码、位图本身等）

}


void kernel_main(void) {
    clear_screen();
    print_LOGO();
    clear_screen();
    kprint("Clean Screen!", 0, 0);
    while(1){}
    // 第1步：设置关键基础设施
    init_physical_memory();   // 内存管理
    // init_kernel_heap();       // 动态分配
    // // 第2步：设置中断系统
    // init_idt();               // 中断描述符表
    // init_pic();               // 中断控制器
    // init_timer(100);          // 定时器中断

    // // 第3步：设置默认中断处理程序
    // for (int i = 0; i < 256; i++) {
    //     set_idt_entry(i, default_interrupt_handler);
    // }

    // // 第4步：加载IDT
    // load_idt();

    // 第5步：现在才安全开中断！
    // asm volatile("sti");
    
    // kprint("中断系统已启动!", 12, 35);
}

// // kernel_main 中按顺序：
// 1. init_physical_memory()   // 内存管理
// 2. init_idt()              // 中断描述符表
// 3. init_pic()              // 中断控制器
// 4. init_timer(100)         // 定时器中断
// 5. sti()                   // 开启中断
// 6. 开始进程管理...