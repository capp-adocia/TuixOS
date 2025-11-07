/* include/Hydrangea/gdt.h */

#ifndef I_H_gdt_H
#define I_H_gdt_H

#include <stddef.h>

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

extern struct gdt_entry gdt_entries[3];

/**
 * 初始化GDT
 */
void init_gdt();


#endif