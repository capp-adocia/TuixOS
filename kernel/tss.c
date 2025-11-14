/* kernel/tss.c */

#include <Silan/tss.h>

struct tss_entry tss;
uint32_t kernel_stack[1024];  // 4KB内核栈


void init_tss(void)
{
    // 设置内核栈指针
    tss.ss0 = 0x10;
    tss.esp0 = (uint32_t)&kernel_stack[1024];
    // 其他字段初始化为0
    tss.esp1 = tss.ss1 = tss.esp2 = tss.ss2 = 0;
    tss.iomap_base = sizeof(struct tss_entry);
}