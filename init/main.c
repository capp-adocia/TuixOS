/* init/main.c 实现C内核 */
#include <Hydrangea/screen.h>
#include <Hydrangea/memory.h>
#include <string.h>
#include <stddef.h>

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


void kernel_main(void) {
    // clear_screen();
    // print_LOGO();
    clear_screen();
    kprintf(0, 0, "%s", "HydrangeaOS v0.01");
    
    // 第1步：设置关键基础设施
    init_physical_memory();   // 内存管理
    uint32_t addr[10];
    // uint32_t p = alloc_pages_discrete(addr, 10);
    // free_pages_discrete(addr, 9);
    // uint32_t p = alloc_pages(10);
    // free_pages(p, 3);
    uint32_t total;
    uint32_t free;
    get_memory_info(&total, &free);
    char t[32], f[32];
    kprintf(2, 0, "%d", total);
    kprintf(3, 0, "%d", free);

    init_kernel_heap();       // 动态分配
    int page_index = 259;
    if (page_is_free(page_index)) {  // 检查1MB后的...
        kprintf(4, 0,"Page %d is free", page_index);
    }
    else kprintf(4, 0,"Page %d is not free", page_index);
     
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
    while(1){}
}

// // kernel_main 中按顺序：
// 1. init_physical_memory()   // 内存管理
// 2. init_idt()              // 中断描述符表
// 3. init_pic()              // 中断控制器
// 4. init_timer(100)         // 定时器中断
// 5. sti()                   // 开启中断
// 6. 开始进程管理...