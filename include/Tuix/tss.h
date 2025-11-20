/* include/Tuix/tss.h */

#ifndef I_T_TSS_H
#define I_T_TSS_H

#include <stddef.h>

struct tss_entry
{
    uint32_t prev_tss;  // 前一个TSS的链接
    uint32_t esp0;      // 内核栈指针
    uint32_t ss0;       // 内核栈段
    uint32_t esp1;      // Ring 1栈指针
    uint32_t ss1;
    uint32_t esp2;      // Ring 2栈指针  
    uint32_t ss2;
    uint32_t cr3;       // 页目录基址
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax, ecx, edx, ebx;
    uint32_t esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldtr;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed));

extern struct tss_entry tss;

/**
 * 初始化tss
 */
void init_tss(void);

#endif