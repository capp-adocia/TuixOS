/* kernel/sys/mulitiboot2.c */

#include "stddef.h"
#include <Tuix/mulitiboot2.h>
#include <Tuix/serial.h>
#include <def.h>

struct system_memory_info mem_info = {0};
static void parse_kernel_info(void);

void parse_multiboot2_info(uint32_t magic, uint32_t addr)
{
    // 验证魔数
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
    {
        serial_printf("错误: 不是Multiboot2引导 魔数: %x\n", magic);
        return;
    }
    //遍历所有Tag
    struct multiboot_tag* tag = (struct multiboot_tag*)(addr + 8);
    int tag_count = 0;

    while (tag->type != MULTIBOOT_TAG_TYPE_END)
    {
        tag_count++;
        #ifdef MEM_INFO
        serial_printf("\nTag #%d: 类型=%x, 大小=%d\n", tag_count, tag->type, tag->size);
        #endif
        switch (tag->type)
        {
            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
            {
                struct multiboot_tag_basic_meminfo* meminfo = 
                    (struct multiboot_tag_basic_meminfo*)tag;
                #ifdef MEM_INFO
                serial_printf("  [基本内存信息]\n");
                serial_printf("    低端内存(0-640KB): %d KB\n", meminfo->mem_lower);
                serial_printf("    高端内存(1MB以上): %d KB\n", meminfo->mem_upper);
                #endif
                mem_info.mem_lower = meminfo->mem_lower;
                mem_info.mem_upper = meminfo->mem_upper;

                #ifdef MEM_INFO
                uint32_t total_kb = meminfo->mem_lower + meminfo->mem_upper;
                serial_printf("    总内存: %d KB (%d MB)\n", total_kb, total_kb / 1024);
                #endif
                break;
            }
            case MULTIBOOT_TAG_TYPE_MMAP:
            {
                struct multiboot_tag_mmap* mmap = (struct multiboot_tag_mmap*)tag;
                #ifdef MEM_INFO
                serial_printf("  [内存映射信息]\n");
                serial_printf("    条目大小: %d\n", mmap->entry_size);
                serial_printf("    条目版本: %d\n", mmap->entry_version);
                #endif
                // 解析内存映射条目
                parse_memory_map_entries(mmap);
                break;
            }
            
            #ifdef MEM_INFO
            case MULTIBOOT_TAG_TYPE_CMDLINE:
            {
                struct multiboot_tag_string* cmdline = (struct multiboot_tag_string*)tag;
                serial_printf("  [命令行] %s\n", cmdline->string);
                break;
            }
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
            {
                struct multiboot_tag_string* bootloader = (struct multiboot_tag_string*)tag;
                serial_printf("  [引导加载器] %s\n", bootloader->string);
                break;
            }
            #endif
            
            default:
                #ifdef MEM_INFO
                serial_printf("  [未知Tag类型]\n");
                #endif
                break;
        }
        
        // 移动到下一个Tag（8字节对齐）
        tag = (struct multiboot_tag*)((uint8_t*)tag + ((tag->size + 7) & ~7));
    }
    
    // print_memory_summary();
    // 解析出内核占用区域
    parse_kernel_info();
}

void parse_memory_map_entries(struct multiboot_tag_mmap* mmap)
{
    uint32_t entry_count = (mmap->size - 16) / mmap->entry_size;

    struct multiboot_mmap_entry* entry = (struct multiboot_mmap_entry*)((uint8_t*)mmap + 16);
    
    // 重置统计
    mem_info.total_physical_memory = 0;
    mem_info.total_available_memory = 0;
    mem_info.first_usable_addr = 0;
    mem_info.last_usable_addr = 0;

    mem_info.memory_map_count = entry_count;

    for (uint32_t i = 0; i < entry_count; i++)
    {
        const char* type_desc;
        UNUSED(type_desc);
        switch (entry->type)
        {
            case 1: type_desc = "可用RAM"; break;
            case 2: type_desc = "保留"; break;
            case 3: type_desc = "ACPI回收"; break;
            case 4: type_desc = "ACPI NVS"; break;
            case 5: type_desc = "坏内存"; break;
            default: type_desc = "未知"; break;
        }
        uint64_t end_addr = entry->addr + entry->len;
        #ifdef MEM_INFO
        serial_printf("    条目%d: %x", i, (uint32_t)entry->addr);
        
        serial_printf(" - %x", (uint32_t)end_addr - 1);
        
        serial_printf(", 大小: %d MB, 类型: %s\n",
                     (uint32_t)(entry->len / (1024 * 1024)),
                     type_desc);
        #endif
        // 保存到内存映射数组
        mem_info.memory_map[i] = *entry;
        
        // 统计总物理内存（所有区域）
        mem_info.total_physical_memory += entry->len;
        
        // 统计可用内存和更新边界
        if (entry->type == 1)
        {  // 可用RAM
            mem_info.total_available_memory += entry->len;
            
            // 更新关键地址边界（只考虑1MB以上的高端内存）
            if (entry->addr >= 0x100000 && entry->addr < 0xFFFFFFFF)
            {
                uint32_t start_32 = (uint32_t)entry->addr;
                uint32_t end_32 = (uint32_t)end_addr;
                
                if (mem_info.first_usable_addr == 0 || start_32 < mem_info.first_usable_addr)
                    mem_info.first_usable_addr = start_32;

                if (end_32 > mem_info.last_usable_addr)
                    mem_info.last_usable_addr = end_32;

            }
        }

        // 移动到下一个条目
        entry = (struct multiboot_mmap_entry*)((uint8_t*)entry + mmap->entry_size);
    }
    #ifdef MEM_INFO
    serial_printf("    总可用内存: %d MB\n", 
                 (uint32_t)(mem_info.total_available_memory / (1024 * 1024)));
    #endif
}

// 打印内存汇总信息
void print_memory_summary(void)
{
    serial_printf("\n=== 内存汇总信息 ===\n");
    serial_printf("总物理内存: %d MB\n", (uint32_t)(mem_info.total_physical_memory / (1024 * 1024)));
    serial_printf("总可用内存: %d MB\n", (uint32_t)(mem_info.total_available_memory / (1024 * 1024)));
    serial_printf("第一个可用地址: %x\n", mem_info.first_usable_addr);
    serial_printf("最后一个可用地址: %x\n", mem_info.last_usable_addr);
    serial_printf("内存映射条目数: %d\n", mem_info.memory_map_count);
}

static void parse_kernel_info(void)
{
    mem_info.kernel_start_addr = (uint32_t)_kernel_start;
    mem_info.ktext_start_addr = (uint32_t)_kernel_text_start;
    mem_info.ktext_end_addr = (uint32_t)_kernel_text_end;
    mem_info.krodata_addr = (uint32_t)_kernel_rodata_start;
    mem_info.krodata_end_addr = (uint32_t)_kernel_rodata_end;
    mem_info.kdata_start_addr = (uint32_t)_kernel_data_start;
    mem_info.kdata_end_addr = (uint32_t)_kernel_data_end;
    mem_info.kbss_start_addr = (uint32_t)_kernel_bss_start;
    mem_info.kbss_end_addr = (uint32_t)_kernel_bss_end;
    mem_info.kernel_end_addr = (uint32_t)_kernel_end;
}
