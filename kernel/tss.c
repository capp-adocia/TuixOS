/* kernel/tss.c */

#include <Tuix/tss.h>
#include <Tuix/mmu.h>
#include <Tuix/sysconf.h>
#include <Tuix/kalloc.h>
#include <Tuix/vm.h>
#include <string.h>

struct tss_entry cpu_tss[MAX_CPUS];

uint32_t kernel_stack[1024];  // 4KB内核栈

/* 注意这里单核默认用第一个tss */
void init_tss(void)
{
    memset(&cpu_tss[cpu_cur_id], 0, sizeof(struct tss_entry));
    // 设置内核栈指针
    cpu_tss[cpu_cur_id].ss0 = 0x10;
    cpu_tss[cpu_cur_id].esp0 = (uint32_t)&kernel_stack[1024];
    cpu_tss[cpu_cur_id].cr3 = (uint32_t)P2V(kpgdir);
    cpu_tss[cpu_cur_id].iomap_base = sizeof(struct tss_entry);
}
