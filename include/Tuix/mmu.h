/* include/Tuix/mmu.h */

#ifndef I_T_MMU_H
#define I_T_MMU_H

#include <stddef.h>

#define MMU_SIZE    4 // 32位cpu 对应的是4字节
#define FREE_STR    0xdeadbeef // kfree时设置
#define PGSIZE      4096    // 页大小
#define PDE_NUM     1024 // 每一个页目录的页目录项个数
#define PTE_NUM     1024 // 每一个页表中的页表项个数

/* 标志位定义 */
#define PTE_P       0x001   // 存在位
#define PTE_W       0x002   // 可写位
#define PTE_U       0x004   // 用户位

/* 页表相关 */
#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1)) // 用于确定分配的大小
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1)) // 用于确定该地址对应的
#define PTXSHIFT    12      // 页目录的偏移量
#define PDXSHIFT    22      // 页表项的偏移量
#define PDX(va)     (((uint32_t)(va) >> PDXSHIFT) & 0x3FF) // 页目录索引
#define PTX(va)     (((uint32_t)(va) >> PTXSHIFT) & 0x3FF) // 页表索引
#define PTE_ADDR(pte)   ((uint32_t)(pte) & ~0xFFF)
#define PTE_FLAGS(pte)  ((uint32_t)(pte) &  0xFFF)

/* 内存布局 */
#define KERNBASE    0x80000000      // 第一个内核虚拟地址
#define KERNLINK (KERNBASE+EXTMEM)  // 内核链接地址
#define EXTMEM      0x100000
#define DEVSPACE    0xFE000000      // 其他设备位于高地址

/* 虚实转换 */
#define V2P(a) (((uint32_t) (a)) - KERNBASE)
#define P2V(a) ((void*)(((char*) (a)) + KERNBASE))

#endif
