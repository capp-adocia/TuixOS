/* include/Tuix/mulitiboot2.h */

#ifndef I_T_M2_H
#define I_T_M2_H

#include <stddef.h>

// Multiboot2 魔数
#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36d76289

// Tag 类型定义
#define MULTIBOOT_TAG_TYPE_END               0
#define MULTIBOOT_TAG_TYPE_CMDLINE           1
#define MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME  2
#define MULTIBOOT_TAG_TYPE_MODULE            3
#define MULTIBOOT_TAG_TYPE_BASIC_MEMINFO     4
#define MULTIBOOT_TAG_TYPE_MMAP              6

// 基本Tag结构
struct multiboot_tag
{
    uint32_t type;
    uint32_t size;
};

// 内存映射Tag
struct multiboot_tag_mmap
{
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    // 后面跟着内存映射条目数组
};

// 内存映射条目
struct multiboot_mmap_entry
{
    uint64_t addr;
    uint64_t len;
    uint32_t type;
    uint32_t zero;
} __attribute__((packed));

// 字符串类型的Tag结构
struct multiboot_tag_string
{
    uint32_t type;
    uint32_t size;
    char string[0];  // 可变长度字符串
};

// 模块信息Tag  
struct multiboot_tag_module
{
    uint32_t type;
    uint32_t size;
    uint32_t mod_start;
    uint32_t mod_end;
    char cmdline[0];  // 命令行字符串
};

struct multiboot_tag_basic_meminfo
{
    uint32_t type;
    uint32_t size;
    uint32_t mem_lower;  // 0-640KB内存大小(KB)
    uint32_t mem_upper;  // 1MB以上内存大小(KB)
};

struct system_memory_info
{
    // 内存映射信息
    struct multiboot_mmap_entry memory_map[32];
    uint32_t memory_map_count;      // 内存映射条目数
    
    // 内存统计
    uint64_t total_physical_memory;
    uint64_t total_available_memory;
    
    // 关键地址边界
    uint32_t first_usable_addr;     // 第一个可用高端内存地址
    uint32_t last_usable_addr;      // 最后一个可用内存地址(不是最大区域的地址)
    // 内核区域信息
    uint32_t kernel_start_addr;     // 内核开始地址
    uint32_t ktext_start_addr;      // 内核text开始地址
    uint32_t ktext_end_addr;        // 内核text结束地址
    uint32_t krodata_addr;          // 内核rodata结束地址
    uint32_t krodata_end_addr;      // 内核rodata结束地址
    uint32_t kdata_start_addr;      // 内核data开始地址
    uint32_t kdata_end_addr;        // 内核data结束地址
    uint32_t kbss_start_addr;       // 内核bss开始地址
    uint32_t kbss_end_addr;         // 内核bss结束地址
    uint32_t kernel_end_addr;       // 内核结束地址

    uint32_t memory_bitmap_addr;    // 内存位图物理地址
    uint32_t memory_bitmap_size;    // 内存位图大小(字节)
    
    // 基本内存信息
    uint32_t mem_lower;             // 0-640KB内存(KB)
    uint32_t mem_upper;             // 1MB以上内存(KB)
};

extern struct system_memory_info mem_info;

/* 记录了内核各个部分的界限 */
extern char _kernel_start[];
extern char _kernel_text_start[], _kernel_text_end[];
extern char _kernel_rodata_start[], _kernel_rodata_end[];
extern char _kernel_data_start[], _kernel_data_end[];
extern char _kernel_bss_start[], _kernel_bss_end[];
extern char _kernel_end[];

/**
 * 解析multiboot2的信息
 * @param magic
 * @param addr
 */
void parse_multiboot2_info(uint32_t magic, uint32_t addr);

/**
 * 解析内存信息
 * @param mmap
 */
void parse_memory_map_entries(struct multiboot_tag_mmap* mmap);

/**
 * 打印内存信息
 */
void print_memory_summary(void);
#endif