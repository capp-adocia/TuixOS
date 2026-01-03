/* include/Tuix/interrupts.h 中断处理函数（软件层面） */

#ifndef I_T_INTERRUPTS_H
#define I_T_INTERRUPTS_H

#include <stddef.h>
#include <Tuix/trap_frame.h>

// 每个中断号对应的处理函数 trap_frame
typedef void (*interrupt_handler_t)(struct trap_frame*);

/**
 * 中断默认处理
 * @param regs 栈帧
 */
void isr_default_handler(struct trap_frame* regs);
// 0: 除零错误
void isr_divide_error_handler(struct trap_frame* regs);
// 1: 调试异常  
void isr_debug_exception_handler(struct trap_frame* regs);
// 2: 非屏蔽中断
void isr_nmi_handler(struct trap_frame* regs);
// 3: 断点
void isr_breakpoint_handler(struct trap_frame* regs);
// 4: 溢出
void isr_overflow_handler(struct trap_frame* regs);
// 5: 边界检查
void isr_bounds_check_handler(struct trap_frame* regs);
// 6: 无效操作码
void isr_invalid_opcode_handler(struct trap_frame* regs);
// 7: 设备不可用
void isr_device_not_available_handler(struct trap_frame* regs);
// 8: 双重故障
void isr_double_fault_handler(struct trap_frame* regs);
// 9: 协处理器段越界
void isr_coprocessor_segment_handler(struct trap_frame* regs);
// 10: 无效TSS
void isr_invalid_tss_handler(struct trap_frame* regs);
// 11: 段不存在
void isr_segment_not_present_handler(struct trap_frame* regs);
// 12: 栈段错误
void isr_stack_segment_fault_handler(struct trap_frame* regs);
// 13: 通用保护错误
void isr_general_protection_fault_handler(struct trap_frame* regs);
// 14: 页错误
void isr_page_fault_handler(struct trap_frame* regs);
// 15: 保留
void isr_reserved_15_handler(struct trap_frame* regs);
// 16: 浮点错误
void isr_floating_point_handler(struct trap_frame* regs);
// 17: 对齐检查
void isr_alignment_check_handler(struct trap_frame* regs);
// 18: 机器检查
void isr_machine_check_handler(struct trap_frame* regs);
// 19: SIMD浮点异常
void isr_simd_floating_point_handler(struct trap_frame* regs);
// 20: 虚拟化异常
void isr_virtualization_handler(struct trap_frame* regs);
// 21: 控制保护异常
void isr_control_protection_handler(struct trap_frame* regs);
// 22: 保留
void isr_reserved_22_handler(struct trap_frame* regs);
// 23: 保留
void isr_reserved_23_handler(struct trap_frame* regs);
// 24: 保留
void isr_reserved_24_handler(struct trap_frame* regs);
// 25: 保留
void isr_reserved_25_handler(struct trap_frame* regs);
// 26: 保留
void isr_reserved_26_handler(struct trap_frame* regs);
// 27: 保留
void isr_reserved_27_handler(struct trap_frame* regs);
// 28: Hypervisor注入异常
void isr_hypervisor_injection_handler(struct trap_frame* regs);
// 29: VMM通信异常
void isr_vmm_communication_handler(struct trap_frame* regs);
// 30: 安全异常
void isr_security_exception_handler(struct trap_frame* regs);
// 31: 保留
void isr_reserved_31_handler(struct trap_frame* regs);
// 32: 定时器中断
void isr_timer_handler(struct trap_frame* regs);
// 33: 键盘中断
void isr_keyboard_handler(struct trap_frame* regs);
// 34: 级联中断
void isr_cascade_handler(struct trap_frame* regs);
// 35: COM2串口
void isr_com2_handler(struct trap_frame* regs);
// 36: COM1串口
void isr_com1_handler(struct trap_frame* regs);
// 37: LPT2并口
void isr_lpt2_handler(struct trap_frame* regs);
// 38: 软盘控制器
void isr_floppy_handler(struct trap_frame* regs);
// 39: LPT1并口
void isr_lpt1_handler(struct trap_frame* regs);
// 40: 实时时钟
void isr_rtc_handler(struct trap_frame* regs);
// 41: 保留
void isr_reserved_41_handler(struct trap_frame* regs);
// 42: 保留
void isr_reserved_42_handler(struct trap_frame* regs);
// 43: 保留
void isr_reserved_43_handler(struct trap_frame* regs);
// 44: PS/2鼠标
void isr_ps2_mouse_handler(struct trap_frame* regs);
// 45: 协处理器（FPU错误）
void isr_fpu_handler(struct trap_frame* regs);
// 46: 主ATA硬盘
void isr_ata1_handler(struct trap_frame* regs);
// 47: 从ATA硬盘
void isr_ata2_handler(struct trap_frame* regs);

#endif
