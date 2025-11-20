/* include/Tuix/gdt.h */

#ifndef I_T_GDT_H
#define I_T_GDT_H

#include <stddef.h>
#include <Tuix/tss.h>

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


/* NULL, 内核代码, 内核数据, 用户代码, 用户数据, TSS */
extern struct gdt_entry gdt_entries[6];

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