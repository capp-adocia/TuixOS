/* kernel/tss.c */

#include <Tuix/page.h>
#include <Tuix/sysconf.h>
#include <Tuix/tss.h>
#include <Tuix/memory.h>

struct tss_entry cpu_tss[MAX_CPUS];

uint32_t kernel_stack[1024];  // 4KB内核栈

/* 注意这里单核默认用第一个tss */
void init_tss(void)
{
    memset(&cpu_tss[cpu_cur_id], 0, sizeof(struct tss_entry));
    // 设置内核栈指针
    cpu_tss[cpu_cur_id].ss0 = 0x10;
    cpu_tss[cpu_cur_id].esp0 = (uint32_t)&kernel_stack[1024];
    cpu_tss[cpu_cur_id].cr3 = (uint32_t)get_cr3();
    cpu_tss[cpu_cur_id].iomap_base = sizeof(struct tss_entry);
}
