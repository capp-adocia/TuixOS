/* init/main.c 实现C内核 */
/* init/main.c 实现C内核 */
#include <Hydrangea/screen.h>
#include <Hydrangea/memory.h>
#include <string.h>
#include <stddef.h>
#include <def.h>

// 1. 先声明所有函数
void kernel_main(void);
void print_LOGO(void);
void test_malloc(void);
void test_compact_debug(void);
void test_compact2(void);

// 2. 常量定义
const char* logo[] = {
    "H   H  Y   Y  DDDD   RRRRR     A    N   N  GGGG  EEEEEE    A  ",
    "H   H   Y Y   D   D  R    R   A A   NN  N G      E        A A ",
    "HHHHH    Y    D   D  RRRRRR  AAAAA  N N N G  GGG EEEEE   AAAAA",
    "H   H    Y    D   D  R  R   A     A N  NN G   GG E      A     A",
    "H   H    Y    DDDD   R  RRR A     A N   N  GGGG  EEEEEE A     A"
};

// 3. 入口点
__attribute__((naked)) void _start(void)
{
    asm volatile(
        "call kernel_main\n"
        "hlt\n"
        "jmp .\n"
    );
}

// 4. 辅助函数
void print_LOGO(void)
{
    int logo_height = sizeof(logo) / sizeof(logo[0]);
    for (int i = 0; i < logo_height; i++) {
        kprint(logo[i], 9 + i, 9);
    }
    kprint(20, 30, "HydrangeaOS v0.01");
    
    volatile int c = 500000;  // 加 volatile 避免被优化
    while(c--) {
        kprint(0, 0, "LOGO!");
    }
    kprint(0, 0, "Done!");
}

// 5. 测试函数集中放在一起
void test_malloc(void)
{
    char* video = (char*)0xB8000;
    
    // 方法1: 使用字符串常量（可能失败）
    kprint(0, 0, "Constant String Test Line 00Constant String Test Line 00");
    kprint(1, 0, "Constant String Test Line 01Constant String Test Line 00");
    kprint(2, 0, "Constant String Test Line 02Constant String Test Line 00");
    kprint(3, 0, "Constant String Test Line 0Constant String Test Line 003");
    kprint(4, 0, "Constant String Test Line 0Constant String Test Line 004");
    kprint(5, 0, "Constant String Test Line 0Constant String Test Line 005");
    kprint(6, 0, "Constant String Test Line 0Constant String Test Line 006");
    kprint(7, 0, "Constant String Test Line 0Constant String Test Line 007");
    kprint(8, 0, "Constant String Test Line 0Constant String Test Line 008");
    kprint(9, 0, "Constant String Test Line 0Constant String Test Line 009");
    kprint(10, 0, "Constant String Test Line 1Constant String Test Line 000");
    kprint(11, 0, "Constant String Test Line 0Constant String Test Line 007");
    kprint(12, 0, "Constant String Test Line 0Constant String Test Line 008");
    kprint(13, 0, "Constant String Test Line 0Constant String Test Line 009");
    kprint(14, 0, "Constant String Test Line 1Constant String Test Line 000");
    kprint(15, 0, "Constant String Test Line 1Constant String Test Line 000");
    kprint(16, 0, "Constant String Test Line 1Constant String Test Line 000");
    kprint(17, 0, "Constant String Test Line 1Constant String Test Line 000");
    kprint(18, 0, "Constant String Test Line 1Constant String Test Line 000");
    kprint(19, 0, "Constant String Test Line 1Constant String Test Line 000");
    kprint(20, 0, "Constant String Test Line 1Constant String Test Line 000");
    kprint(21, 0, "Constant String Test Line 1Constant String Test Line 000");
    kprint(22, 0, "Constant String Test Line 1Constant String Test Line 000");
    kprint(23, 0, "Constant String Test Line 1Constant String Test Line 000");
    kprint(24, 0, "Constant String Test Line 1Constant String Test Line 000");
}

void kernel_main(void) {
    clear_screen();
    
    // 基础显示
    kprintf(0, 0, "%s", "HydrangeaOS v0.01");
    
    // 内存初始化
    init_physical_memory();
    uint32_t total, free;
    get_memory_info(&total, &free);
    kprintf(2, 0, "Total: %d KB", total);
    kprintf(3, 0, "Free: %d KB", free);

    init_kernel_heap();
    
    // 基础检查
    int page_index = 259;
    if (page_is_free(page_index)) {
        kprintf(4, 0, "Page %d is free", page_index);
    } else {
        kprintf(4, 0, "Page %d is used", page_index);
    }
    
    // 阶段4: 运行测试（一次只运行一个）
    test_malloc();
    // test_compact_debug();
    // test_compact2();
    
    // init_idt();
    // init_pic();
    // init_timer(100);
    // asm volatile("sti");
    
    while(1) {}
}