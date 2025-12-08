/* kernel/interrupts.c */

#include <Tuix/interrupts.h>
#include <Tuix/screen.h>
#include <Tuix/io.h>
#include <Tuix/keyboard.h>
#include <Tuix/timer.h>
#include <Tuix/serial.h>

void isr_default_handler(struct pt_regs* regs)
{
    if(regs->int_no < 32){
        kprint(15, 0, "Ex:\n");
        
        char num_str[3];
        num_str[0] = '0' + (regs->int_no / 10);
        num_str[1] = '0' + (regs->int_no % 10); 
        num_str[2] = '\0';
        kprint(15, 3, num_str);
        while(1);
    }
}
/*
    函数名待修改:用isr_开头后面写上功能含义
*/
// 0: 除零错误 - 可恢复，设置默认结果
void isr_divide_error_handler(struct pt_regs* regs)
{
    serial_printf("Divide Error at EIP: %x - Fixed\n", regs->eip);
    regs->eax = 0;  // 设置结果为0，继续执行
    regs->eip += 2;
}

// 1: 调试异常 - 可恢复，用于调试器
void isr_debug_exception_handler(struct pt_regs* regs)
{
    serial_printf("Debug Exception at EIP: %x - Continue\n", regs->eip);
    // 单步执行，继续
}

// 2: 非屏蔽中断 - 严重硬件错误
void isr_nmi_handler(struct pt_regs* regs)
{
    serial_printf("NMI Interrupt - Hardware Failure\n");
    serial_printf("System Halted\n");
    while(1) __asm__ volatile("cli; hlt");  // 停机
}

// 3: 断点 - 可恢复，用于调试
void isr_breakpoint_handler(struct pt_regs* regs)
{
    serial_printf("Breakpoint at EIP: %x - Continue\n", regs->eip);
    // 调试断点，继续执行
}

// 4: 溢出 - 可恢复，清除标志
void isr_overflow_handler(struct pt_regs* regs)
{
    serial_printf("Overflow at EIP: %x - Cleared\n", regs->eip);
    // 溢出标志会被自动处理，继续执行
}

// 5: 边界检查 - 可恢复，修复索引
void isr_bounds_check_handler(struct pt_regs* regs)
{
    serial_printf("Bounds Check at EIP: %x - Fixed\n", regs->eip);
    // 可以修复边界索引，继续执行
}

// 6: 无效操作码 - 严重，无法恢复
void isr_invalid_opcode_handler(struct pt_regs* regs)
{
    serial_printf("Invalid Opcode at EIP: %x\n", regs->eip);
    serial_printf("Unrecoverable - System Halted\n");
    while(1) __asm__ volatile("cli; hlt");
}

// 7: 设备不可用 - 可恢复，模拟或禁用
void isr_device_not_available_handler(struct pt_regs* regs)
{
    serial_printf("Device Not Available\n");
    __asm__ volatile("mov %cr0, %eax; and $0xFFFFFFFB, %eax; mov %eax, %cr0\n");
}

// 8: 双重故障 - 严重系统错误
void isr_double_fault_handler(struct pt_regs* regs)
{
    serial_printf("Double Fault! Error: %x\n", regs->err_code);
    while(1) __asm__ volatile("cli; hlt");
}

// 9: 协处理器段越界
void isr_coprocessor_segment_handler(struct pt_regs* regs)
{
    serial_printf("Coprocessor Segment Overrun - Fixed\n");
    // 继续执行
}

// 10: 无效TSS - 系统配置错误
void isr_invalid_tss_handler(struct pt_regs* regs)
{
    serial_printf("Invalid TSS. Error: %x\n", regs->err_code);
    serial_printf("Kernel Panic - System Halted\n");
    while(1) __asm__ volatile("cli; hlt");
}

// 11: 段不存在, 重新加载段
void isr_segment_not_present_handler(struct pt_regs* regs)
{
    serial_printf("Segment Not Present. Error: %x\n", regs->err_code);
    // 可以重新加载段寄存器或修复GDT
    // 这里简单继续,在实际OS中需要更复杂处理
}

// 12: 栈段错误 - 严重内存错误
void isr_stack_segment_fault_handler(struct pt_regs* regs)
{
    serial_printf("Stack Segment Fault. Error: %x\n", regs->err_code);
    serial_printf("Stack Corrupted - System Halted\n");
    while(1) __asm__ volatile("cli; hlt");
}

// 13: 通用保护错误 - 严重内存/权限错误
void isr_general_protection_fault_handler(struct pt_regs* regs)
{
    serial_printf("General Protection Fault! Error: %x\n", regs->err_code);
    serial_printf("System Halted\n");
    while(1) __asm__ volatile("cli; hlt");
}

// 14: 页错误 - 可恢复，处理缺页
void isr_page_fault_handler(struct pt_regs* regs)
{
    uint32_t fault_addr;
    __asm__ volatile("mov %%cr2, %0" : "=r"(fault_addr));
    serial_printf("Error Code: %x\n", regs->err_code);
    if(regs->err_code & 0x1)
        serial_printf("Page Fault: 页面不存在 %x\n", fault_addr);
    else
        serial_printf("Page Fault: 权限错误 %x - Halting\n", fault_addr);
        // 获取当前特权级
    // 解析错误码
    serial_printf("  P=%d (Page %s)\n", 
                  (regs->err_code & 0x1) ? 1 : 0,
                  (regs->err_code & 0x1) ? "Present" : "Not Present");
    serial_printf("  W/R=%d (%s operation)\n",
                  (regs->err_code & 0x2) ? 1 : 0,
                  (regs->err_code & 0x2) ? "Write" : "Read");
    serial_printf("  U/S=%d (%s mode)\n",
                  (regs->err_code & 0x4) ? 1 : 0,
                  (regs->err_code & 0x4) ? "User" : "Supervisor");
    serial_printf("  RSVD=%d (Reserved bit %s)\n",
                  (regs->err_code & 0x8) ? 1 : 0,
                  (regs->err_code & 0x8) ? "violation" : "OK");
    serial_printf("  I/D=%d (%s fetch)\n",
                  (regs->err_code & 0x10) ? 1 : 0,
                  (regs->err_code & 0x10) ? "Instruction" : "Data");
    uint16_t cs;
    __asm__ volatile("mov %%cs, %0" : "=r"(cs));
    serial_printf("Current CS: %x, CPL=%d\n", cs, cs & 0x3);
    
    // 获取EIP
    uint32_t eip;
    __asm__ volatile("call 1f\n1: pop %0" : "=r"(eip));
    serial_printf("Faulting EIP: %x\n", eip);
    
    // 检查页表项
    uint32_t pde_index = fault_addr >> 22;
    uint32_t pt_index = (fault_addr >> 12) & 0x3FF;
    
    serial_printf("Page Table Entry: PDE[%d] PTE[%d]\n", pde_index, pt_index);
    
    // 读取CR3
    uint32_t cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
    serial_printf("CR3: %x\n", cr3);
    
    // 如果U/S=0，说明是权限问题
    if ((regs->err_code & 0x4) == 0) {
        serial_printf("\n!!! CONFIRMED: User-mode access to supervisor page !!!\n");
        serial_printf("You need to set U/S=1 in page table for address %x\n", fault_addr);
    }
    
    // 检查当前 CR3
    uint32_t cr3_val;
    __asm__ volatile("mov %%cr3, %0" : "=r"(cr3_val));
    serial_printf("Current CR3: 0x%x\n", cr3_val);
    
    // 检查页表项
    uint32_t pd_idx = fault_addr >> 22;
    uint32_t pt_idx = (fault_addr >> 12) & 0x3FF;
    
    uint32_t* page_dir = (uint32_t*)cr3_val;
    uint32_t pde = page_dir[pd_idx];
    
    serial_printf("PDE[%d]: 0x%x\n", pd_idx, pde);
    serial_printf("  PDE Present: %d\n", (pde >> 0) & 1);
    
    if (pde & 0x1) {
        uint32_t* page_table = (uint32_t*)(pde & 0xFFFFF000);
        uint32_t pte = page_table[pt_idx];
        
        serial_printf("PTE[%d]: 0x%x\n", pt_idx, pte);
        serial_printf("  PTE Present: %d\n", (pte >> 0) & 1);
        serial_printf("  PTE User: %d\n", (pte >> 2) & 1);
        serial_printf("  Maps to phys: 0x%x\n", pte & 0xFFFFF000);
        
        // 如果 PTE 显示 Present=1，但 CPU 说 P=0，说明是 TLB 问题
        if ((pte & 0x1) && !(regs->err_code & 0x1)) {
            serial_printf("!!! TLB INCONSISTENCY !!!\n");
            serial_printf("PTE says Present=1, but CPU got P=0\n");
        }
    } else {
        serial_printf("ERROR: Page table not present!\n");
    }

    while(1) __asm__ volatile("cli; hlt");
}

// 15: 保留
void isr_reserved_15_handler(struct pt_regs* regs)
{
    serial_printf("Reserved Exception 15 - Ignored\n");
    // 继续执行
}

// 16: 浮点错误 - 可恢复，清除状态
void isr_floating_point_handler(struct pt_regs* regs)
{
    serial_printf("Floating Point Exception - Cleared\n");
    // 清除FPU状态字，继续执行
    __asm__ volatile("fnclex\n");
}

// 17: 对齐检查 - 可修复
void isr_alignment_check_handler(struct pt_regs* regs)
{
    serial_printf("Alignment Check. Error: %x - Fixed\n", regs->err_code);
    // 可以修复对齐，继续执行
}

// 18: 机器检查 - 严重硬件错误
void isr_machine_check_handler(struct pt_regs* regs)
{
    serial_printf("Machine Check - Hardware Failure\n");
    serial_printf("Critical Error - System Halted\n");
    while(1) __asm__ volatile("cli; hlt");
}

// 19-31: 保留和特定平台异常

// 19: SIMD浮点异常
void isr_simd_floating_point_handler(struct pt_regs* regs)
{
    serial_printf("SIMD Floating Point Exception - Cleared\n");
    
    uint32_t mxcsr_value = 0x1F80;  // 默认MXCSR值
    __asm__ volatile("ldmxcsr %0" : : "m"(mxcsr_value));
}

// 20: 虚拟化异常
void isr_virtualization_handler(struct pt_regs* regs)
{
    serial_printf("Virtualization Exception - Ignored\n");
    // 虚拟化相关异常，在没有虚拟化支持时忽略
}

// 21: 控制保护异常
void isr_control_protection_handler(struct pt_regs* regs)
{
    serial_printf("Control Protection Exception. Error: %x\n", regs->err_code);
    // CET（控制流执行技术）相关，可以修复或终止进程
}

// 22: 保留
void isr_reserved_22_handler(struct pt_regs* regs)
{
    serial_printf("Reserved Exception 22 - Ignored\n");
}

// 23: 保留
void isr_reserved_23_handler(struct pt_regs* regs)
{
    serial_printf("Reserved Exception 23 - Ignored\n");
}

// 24: 保留
void isr_reserved_24_handler(struct pt_regs* regs)
{
    serial_printf("Reserved Exception 24 - Ignored\n");
}

// 25: 保留
void isr_reserved_25_handler(struct pt_regs* regs)
{
    serial_printf("Reserved Exception 25 - Ignored\n");
}

// 26: 保留
void isr_reserved_26_handler(struct pt_regs* regs)
{
    serial_printf("Reserved Exception 26 - Ignored\n");
}

// 27: 保留
void isr_reserved_27_handler(struct pt_regs* regs)
{
    serial_printf("Reserved Exception 27 - Ignored\n");
}

// 28: Hypervisor注入异常
void isr_hypervisor_injection_handler(struct pt_regs* regs)
{
    serial_printf("Hypervisor Injection Exception - Ignored\n");
    // 虚拟化相关，在没有hypervisor时忽略
}

// 29: VMM通信异常
void isr_vmm_communication_handler(struct pt_regs* regs)
{
    serial_printf("VMM Communication Exception - Ignored\n");
    // 虚拟化管理程序通信异常
}

// 30: 安全异常
void isr_security_exception_handler(struct pt_regs* regs)
{
    serial_printf("Security Exception. Error: %x\n", regs->err_code);
    serial_printf("Security violation detected\n");
    // 安全相关异常，需小心处理
}

// 31: 保留
void isr_reserved_31_handler(struct pt_regs* regs)
{
    serial_printf("Reserved Exception 31 - Ignored\n");
}

// 32: 定时器中断
void isr_timer_handler(struct pt_regs* regs)
{
    timer_handler(regs);
}

// 33: 键盘中断
void isr_keyboard_handler(struct pt_regs* regs)
{
    keyboard_handler(regs);
}

// 34: 级联中断
void isr_cascade_handler(struct pt_regs* regs)
{

}

// 35: COM2串口
void isr_com2_handler(struct pt_regs* regs)
{

}

// 36: COM1串口
void isr_com1_handler(struct pt_regs* regs)
{

}

// 37: LPT2并口
void isr_lpt2_handler(struct pt_regs* regs)
{

}

// 38: 软盘控制器
void isr_floppy_handler(struct pt_regs* regs)
{

}

// 39: LPT1并口
void isr_lpt1_handler(struct pt_regs* regs)
{

}

// 40: 实时时钟
void isr_rtc_handler(struct pt_regs* regs)
{

}

// 41: 保留
void isr_reserved_41_handler(struct pt_regs* regs)
{

}

// 42: 保留
void isr_reserved_42_handler(struct pt_regs* regs)
{

}

// 43: 保留
void isr_reserved_43_handler(struct pt_regs* regs)
{

}

// 44: PS/2鼠标
void isr_ps2_mouse_handler(struct pt_regs* regs)
{

}

// 45: 协处理器（FPU错误）
void isr_fpu_handler(struct pt_regs* regs)
{

}

// 46: 主ATA硬盘
void isr_ata1_handler(struct pt_regs* regs)
{

}

// 47: 从ATA硬盘
void isr_ata2_handler(struct pt_regs* regs)
{

}
