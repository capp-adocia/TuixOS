/* include/Silan/page.h */

#ifndef I_S_PAGE_H
#define I_S_PAGE_H

#include <stddef.h>

#define PDE_NUM 1024 // 页目录项个数
#define PT_NUM 1024 // 页表个数
#define PTE_NUM 1024 // 一个页表中的页表项个数
#define PT_SIZE 4096 // 页表大小
/* 标志位定义 */
#define PRESENT_BIT    (1 << 0)  // 位0: 存在位：是否存在
#define READ_WRITE_BIT (1 << 1)  // 位1: 读写位：可读可写  
#define USER_BIT       (1 << 2)  // 位2: 用户位：用户是否可以访问

// 页目录数组
extern uint32_t page_directory[1024] __attribute__((aligned(4096)));

/**
 * 启用分页
 */
void init_page(void);

/**
 * 根据页表索引创建新页表
 * @param pd_index 页目录索引
 * @return 新页表的地址
 */
uint32_t* create_page_table(uint32_t pd_index);

/**
 * 虚拟地址获取对应的页表地址
 * @param virtual_addr 虚拟地址
 * @return 页表的地址
 */
uint32_t* get_page_table(uint32_t virtual_addr);

/**
 * 修改页表项
 * @param virtual_addr 虚拟地址
 * @param physical_addr 物理地址
 * @param flags 标志
 * @return 新页表的地址
 */
void map_page(uint32_t virtual_addr, uint32_t physical_addr, uint32_t flags);

/**
 * 虚拟地址获取对应的页目录号
 * @param virtual_addr 虚拟地址
 * @return 页目录号
 */
inline uint32_t get_pd_index(uint32_t virtual_addr)
{
    return (virtual_addr >> 22) & 0x3FF;
}

/**
 * 虚拟地址获取对应的页表索引号
 * @param virtual_addr 虚拟地址
 * @return 页表索引号
 */
inline uint32_t get_pt_index(uint32_t virtual_addr)
{
    return (virtual_addr >> 12) & 0x3FF;
}

/**
 * 虚拟地址获取对应的页内偏移
 * @param virtual_addr 虚拟地址
 * @return 页内偏移
 */
inline uint32_t get_pt_offset(uint32_t virtual_addr)
{
    return (virtual_addr) & 0xFFF;
}

/**
 * 启用硬件分页
 */
void enable_paging(void);

/**
 * 检查指定虚拟地址的映射
 * @param virtual_addr 虚拟地址
 */
void check_mapping(uint32_t virtual_addr);

/**
 * 取消指定虚拟地址的映射
 * @param virtual_addr 虚拟地址
 */
void unmap_page(uint32_t virtual_addr);

/**
 * 计算地址占用页数
 * @param start_addr 起始地址
 * @param end_addr   结束地址 
 */
uint32_t calculate_page_count(uint32_t start_addr, uint32_t end_addr);

/**
 * 设置内核页保护
 */
void setup_kernel_page_protection(void);


#endif