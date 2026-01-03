/* include/Tuix/gdt.h */

#ifndef I_T_GDT_H
#define I_T_GDT_H

#include <stddef.h>

// 段索引
#define KER_CI 1
#define KER_DI 2
#define USR_CI 3
#define USR_DI 4
#define TSS_I  5
// 各段位置
#define KER_CS KER_CI << 3 // 0x08
#define KER_DS KER_DI << 3 // 0x10
#define USR_CS USR_CI << 3 // 0x18
#define USR_DS USR_DI << 3 // 0x20
#define TSS_S  TSS_I  << 3 // 0x28

#define USR_DPL 3

#define FL_IF   0x00000200      // 中断使能位

struct gdt_entry
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

struct gdt_ptr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct tss_entry
{
    uint32_t prev_tss;  // 前一个TSS的链接
    uint32_t esp0;      // 内核栈指针
    uint16_t ss0;       // 内核栈段
    uint16_t padding1;
    uint32_t esp1;      // Ring 1栈指针
    uint16_t ss1;
    uint16_t padding2;
    uint32_t esp2;      // Ring 2栈指针  
    uint16_t ss2;
    uint16_t padding3;
    void* cr3;       // 页目录基址
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax, ecx, edx, ebx;
    uint32_t* esp, ebp;
    uint32_t esi, edi;
    uint16_t es;
    uint16_t padding4;
    uint16_t cs;
    uint16_t padding5;
    uint16_t ss;
    uint16_t padding6;
    uint16_t ds;
    uint16_t padding7;
    uint16_t fs;
    uint16_t padding8;
    uint16_t gs;
    uint16_t padding9;
    uint16_t ldtr;
    uint16_t padding10;
    uint16_t trap;
    uint16_t iomap_base;
};

/**
 * 初始化GDT
 */
void init_gdt();

/**
 * 设置tss字段
 * @param num
 * @param base
 * @param limit
 * @param access
 * @param granularity
 */
void set_tss_entry(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);


#endif
