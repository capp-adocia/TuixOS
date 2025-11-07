/* include/Silan/page.h */

#ifndef I_S_PAGE_H
#define I_S_PAGE_H

#include <stddef.h>

#define PDE_NUM 1024 // 页目录项个数
#define PT_NUM 1024 // 页表个数
#define PTE_NUM 1024 * 1024 // 页表项个数
#define PT_SIZE 4096 // 页表大小
/* 标志位定义 */
#define PRESENT_BIT    (1 << 0)  // 位0: 存在位：是否存在
#define READ_WRITE_BIT (1 << 1)  // 位1: 读写位：可读可写  
#define USER_BIT       (1 << 2)  // 位2: 用户位：用户是否可以访问

/* 记录了内核各个部分的界限 */
extern char _kernel_text_start[], _kernel_text_end[];
extern char _kernel_rodata_start[], _kernel_rodata_end[];
extern char _kernel_data_start[], _kernel_data_end[];
extern char _kernel_bss_start[], _kernel_bss_end[];
extern char _kernel_end[];

/**
 * 启用分页
 */
void init_page();

/**
 * 计算地址占用页数
 * @param start_addr 起始地址
 * @param end_addr   结束地址 
 */
uint32_t calculate_page_count(uint32_t start_addr, uint32_t end_addr);

/**
 * 设置内核页保护
 */
void setup_kernel_page_protection();

/**
 * 启用硬件分页
 */
void enable_paging();

#endif