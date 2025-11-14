/* kernel/interrupts.c */

#include <Silan/interrupts.h>
#include <Silan/screen.h>
#include <Silan/io.h>
#include <stddef.h>
#include <Silan/keyboard.h>
#include <Silan/timer.h>
#include <Silan/serial.h>

void isr_default_handler(struct interrupt_frame* frame)
{
    if(frame->int_no < 32){
        kprint(15, 0, "Ex:\n");
        
        char num_str[3];
        num_str[0] = '0' + (frame->int_no / 10);
        num_str[1] = '0' + (frame->int_no % 10); 
        num_str[2] = '\0';
        kprint(15, 3, num_str);
        while(1);
    }
}
/*
    函数名待修改:用isr_开头后面写上功能含义
*/
// 0: 除零错误 - 可恢复，设置默认结果
void isr_divide_error_handler(struct interrupt_frame* frame)
{
    serial_printf("Divide Error at EIP: %x - Fixed\n", frame->eip);
    frame->eax = 0;  // 设置结果为0，继续执行
    frame->eip += 2;
}

// 1: 调试异常 - 可恢复，用于调试器
void isr_debug_exception_handler(struct interrupt_frame* frame)
{
    serial_printf("Debug Exception at EIP: %x - Continue\n", frame->eip);
    // 单步执行，继续
}

// 2: 非屏蔽中断 - 严重硬件错误
void isr_nmi_handler(struct interrupt_frame* frame)
{
    serial_printf("NMI Interrupt - Hardware Failure\n");
    serial_printf("System Halted\n");
    while(1) __asm__ volatile("cli; hlt");  // 停机
}

// 3: 断点 - 可恢复，用于调试
void isr_breakpoint_handler(struct interrupt_frame* frame)
{
    serial_printf("Breakpoint at EIP: %x - Continue\n", frame->eip);
    // 调试断点，继续执行
}

// 4: 溢出 - 可恢复，清除标志
void isr_overflow_handler(struct interrupt_frame* frame)
{
    serial_printf("Overflow at EIP: %x - Cleared\n", frame->eip);
    // 溢出标志会被自动处理，继续执行
}

// 5: 边界检查 - 可恢复，修复索引
void isr_bounds_check_handler(struct interrupt_frame* frame)
{
    serial_printf("Bounds Check at EIP: %x - Fixed\n", frame->eip);
    // 可以修复边界索引，继续执行
}

// 6: 无效操作码 - 严重，无法恢复
void isr_invalid_opcode_handler(struct interrupt_frame* frame)
{
    serial_printf("Invalid Opcode at EIP: %x\n", frame->eip);
    serial_printf("Unrecoverable - System Halted\n");
    while(1) __asm__ volatile("cli; hlt");
}

// 7: 设备不可用 - 可恢复，模拟或禁用
void isr_device_not_available_handler(struct interrupt_frame* frame)
{
    serial_printf("Device Not Available at EIP: %x\n", frame->eip);
    __asm__ volatile("mov %cr0, %eax; and $0xFFFFFFFB, %eax; mov %eax, %cr0\n");
}

// 8: 双重故障 - 严重系统错误
void isr_double_fault_handler(struct interrupt_frame* frame)
{
    serial_printf("Double Fault! Error: %x\n", frame->err_code);
    serial_printf("EIP: %x, System Halted\n", frame->eip);
    while(1) __asm__ volatile("cli; hlt");
}

// 9: 协处理器段越界
void isr_coprocessor_segment_handler(struct interrupt_frame* frame)
{
    serial_printf("Coprocessor Segment Overrun - Fixed\n");
    // 继续执行
}

// 10: 无效TSS - 系统配置错误
void isr_invalid_tss_handler(struct interrupt_frame* frame)
{
    serial_printf("Invalid TSS. Error: %x\n", frame->err_code);
    serial_printf("Kernel Panic - System Halted\n");
    while(1) __asm__ volatile("cli; hlt");
}

// 11: 段不存在, 重新加载段
void isr_segment_not_present_handler(struct interrupt_frame* frame)
{
    serial_printf("Segment Not Present. Error: %x\n", frame->err_code);
    // 可以重新加载段寄存器或修复GDT
    // 这里简单继续,在实际OS中需要更复杂处理
}

// 12: 栈段错误 - 严重内存错误
void isr_stack_segment_fault_handler(struct interrupt_frame* frame)
{
    serial_printf("Stack Segment Fault. Error: %x\n", frame->err_code);
    serial_printf("Stack Corrupted - System Halted\n");
    while(1) __asm__ volatile("cli; hlt");
}

// 13: 通用保护错误 - 严重内存/权限错误
void isr_general_protection_fault_handler(struct interrupt_frame* frame)
{
    serial_printf("General Protection Fault! Error: %x\n", frame->err_code);
    serial_printf("EIP: %x, CS: %x - System Halted\n", frame->eip, frame->cs);
    while(1) __asm__ volatile("cli; hlt");
}

// 14: 页错误 - 可恢复，处理缺页
void isr_page_fault_handler(struct interrupt_frame* frame)
{
    uint32_t fault_addr;
    __asm__ volatile("mov %%cr2, %0" : "=r"(fault_addr));
    serial_printf("Error Code: %x\n", frame->err_code);
    if(frame->err_code & 0x1)
        serial_printf("Page Fault: 页面不存在 %x\n", fault_addr);
    else
        serial_printf("Page Fault: 权限错误 %x - Halting\n", fault_addr);
    
    while(1) __asm__ volatile("cli; hlt");
}

// 15: 保留
void isr_reserved_15_handler(struct interrupt_frame* frame)
{
    serial_printf("Reserved Exception 15 - Ignored\n");
    // 继续执行
}

// 16: 浮点错误 - 可恢复，清除状态
void isr_floating_point_handler(struct interrupt_frame* frame)
{
    serial_printf("Floating Point Exception - Cleared\n");
    // 清除FPU状态字，继续执行
    __asm__ volatile("fnclex\n");
}

// 17: 对齐检查 - 可修复
void isr_alignment_check_handler(struct interrupt_frame* frame)
{
    serial_printf("Alignment Check. Error: %x - Fixed\n", frame->err_code);
    // 可以修复对齐，继续执行
}

// 18: 机器检查 - 严重硬件错误
void isr_machine_check_handler(struct interrupt_frame* frame)
{
    serial_printf("Machine Check - Hardware Failure\n");
    serial_printf("Critical Error - System Halted\n");
    while(1) __asm__ volatile("cli; hlt");
}

// 19-31: 保留和特定平台异常

// 19: SIMD浮点异常
void isr_simd_floating_point_handler(struct interrupt_frame* frame)
{
    serial_printf("SIMD Floating Point Exception - Cleared\n");
    
    uint32_t mxcsr_value = 0x1F80;  // 默认MXCSR值
    __asm__ volatile("ldmxcsr %0" : : "m"(mxcsr_value));
}

// 20: 虚拟化异常
void isr_virtualization_handler(struct interrupt_frame* frame)
{
    serial_printf("Virtualization Exception - Ignored\n");
    // 虚拟化相关异常，在没有虚拟化支持时忽略
}

// 21: 控制保护异常
void isr_control_protection_handler(struct interrupt_frame* frame)
{
    serial_printf("Control Protection Exception. Error: %x\n", frame->err_code);
    // CET（控制流执行技术）相关，可以修复或终止进程
}

// 22: 保留
void isr_reserved_22_handler(struct interrupt_frame* frame)
{
    serial_printf("Reserved Exception 22 - Ignored\n");
}

// 23: 保留
void isr_reserved_23_handler(struct interrupt_frame* frame)
{
    serial_printf("Reserved Exception 23 - Ignored\n");
}

// 24: 保留
void isr_reserved_24_handler(struct interrupt_frame* frame)
{
    serial_printf("Reserved Exception 24 - Ignored\n");
}

// 25: 保留
void isr_reserved_25_handler(struct interrupt_frame* frame)
{
    serial_printf("Reserved Exception 25 - Ignored\n");
}

// 26: 保留
void isr_reserved_26_handler(struct interrupt_frame* frame)
{
    serial_printf("Reserved Exception 26 - Ignored\n");
}

// 27: 保留
void isr_reserved_27_handler(struct interrupt_frame* frame)
{
    serial_printf("Reserved Exception 27 - Ignored\n");
}

// 28: Hypervisor注入异常
void isr_hypervisor_injection_handler(struct interrupt_frame* frame)
{
    serial_printf("Hypervisor Injection Exception - Ignored\n");
    // 虚拟化相关，在没有hypervisor时忽略
}

// 29: VMM通信异常
void isr_vmm_communication_handler(struct interrupt_frame* frame)
{
    serial_printf("VMM Communication Exception - Ignored\n");
    // 虚拟化管理程序通信异常
}

// 30: 安全异常
void isr_security_exception_handler(struct interrupt_frame* frame)
{
    serial_printf("Security Exception. Error: %x\n", frame->err_code);
    serial_printf("Security violation detected\n");
    // 安全相关异常，需小心处理
}

// 31: 保留
void isr_reserved_31_handler(struct interrupt_frame* frame)
{
    serial_printf("Reserved Exception 31 - Ignored\n");
}

// 32: 定时器中断
void isr_timer_handler(struct interrupt_frame* frame)
{
    timer_handler(frame);
}

// 33: 键盘中断
void isr_keyboard_handler(struct interrupt_frame* frame)
{
    keyboard_handler(frame);
}

// 34: 级联中断
void isr_cascade_handler(struct interrupt_frame* frame)
{

}

// 35: COM2串口
void isr_com2_handler(struct interrupt_frame* frame)
{

}

// 36: COM1串口
void isr_com1_handler(struct interrupt_frame* frame)
{

}

// 37: LPT2并口
void isr_lpt2_handler(struct interrupt_frame* frame)
{

}

// 38: 软盘控制器
void isr_floppy_handler(struct interrupt_frame* frame)
{

}

// 39: LPT1并口
void isr_lpt1_handler(struct interrupt_frame* frame)
{

}

// 40: 实时时钟
void isr_rtc_handler(struct interrupt_frame* frame)
{

}

// 41: 保留
void isr_reserved_41_handler(struct interrupt_frame* frame)
{

}

// 42: 保留
void isr_reserved_42_handler(struct interrupt_frame* frame)
{

}

// 43: 保留
void isr_reserved_43_handler(struct interrupt_frame* frame)
{

}

// 44: PS/2鼠标
void isr_ps2_mouse_handler(struct interrupt_frame* frame)
{

}

// 45: 协处理器（FPU错误）
void isr_fpu_handler(struct interrupt_frame* frame)
{

}

// 46: 主ATA硬盘
void isr_ata1_handler(struct interrupt_frame* frame)
{

}

// 47: 从ATA硬盘
void isr_ata2_handler(struct interrupt_frame* frame)
{

}