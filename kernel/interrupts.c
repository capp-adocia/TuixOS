/* kernel/interrupts.c */

#include <Hydrangea/interrupts.h>
#include <Hydrangea/screen.h>

void isr_default(struct interrupt_frame* frame)
{
    if(frame->int_no < 32){
        kprint(15, 0, "Ex:");
        
        char num_str[3];
        num_str[0] = '0' + (frame->int_no / 10);
        num_str[1] = '0' + (frame->int_no % 10); 
        num_str[2] = '\0';
        kprint(15, 3, num_str);
        while(1);
    }
}

// 0: 除零错误
void isr_0_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Divide Error at EIP: %x", frame->eip);
    while(1);
}

// 1: 调试异常
void isr_1_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Debug Exception at EIP: %x", frame->eip);
    while(1);
}

// 2: 非屏蔽中断
void isr_2_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "NMI Interrupt");
    while(1);
}

// 3: 断点
void isr_3_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Breakpoint at EIP: %x", frame->eip);
    while(1);
}

// 4: 溢出
void isr_4_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Overflow at EIP: %x", frame->eip);
    while(1);
}

// 5: 边界检查
void isr_5_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Bounds Check at EIP: %x", frame->eip);
    while(1);
}

// 6: 无效操作码
void isr_6_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Invalid Opcode at EIP: %x", frame->eip);
    while(1);
}

// 7: 设备不可用
void isr_7_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Device Not Available at EIP: %x", frame->eip);
    while(1);
}

// 8: 双重故障
void isr_8_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Double Fault! Error: %x", frame->err_code);
    kprintf(13, 0, "EIP: %x, ESP: %x", frame->eip, frame->esp);
    while(1);
}

// 9: 协处理器段越界
void isr_9_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Coprocessor Segment Overrun");
    while(1);
}

// 10: 无效TSS
void isr_10_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Invalid TSS. Error: %x", frame->err_code);
    while(1);
}

// 11: 段不存在
void isr_11_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Segment Not Present. Error: %x", frame->err_code);
    while(1);
}

// 12: 栈段错误
void isr_12_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Stack Segment Fault. Error: %x", frame->err_code);
    while(1);
}

// 13: 通用保护错误
void isr_13_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "General Protection Fault! Error: %x", frame->err_code);
    kprintf(13, 0, "EIP: %x, CS: %x", frame->eip, frame->cs);
    while(1);
}

// 14: 页错误
void isr_14_c(struct interrupt_frame* frame) {
    uint32_t fault_addr;
    __asm__ volatile("mov %%cr2, %0" : "=r"(fault_addr));
    kprintf(12, 0, "Page Fault @ %x", fault_addr);
    kprintf(13, 0, "Error: %x, EIP: %x", frame->err_code, frame->eip);
    while(1);
}

// 15: 保留
void isr_15_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Reserved Exception 15");
    while(1);
}

// 16: 浮点错误
void isr_16_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Floating Point Exception");
    while(1);
}

// 17: 对齐检查
void isr_17_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Alignment Check. Error: %x", frame->err_code);
    while(1);
}

// 18: 机器检查
void isr_18_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Machine Check");
    while(1);
}

// 19-31: 保留和特定平台异常
void isr_19_c(struct interrupt_frame* frame) { kprintf(12, 0, "Reserved 19"); while(1); }
void isr_20_c(struct interrupt_frame* frame) { kprintf(12, 0, "Reserved 20"); while(1); }
void isr_21_c(struct interrupt_frame* frame) { kprintf(12, 0, "Reserved 21"); while(1); }
void isr_22_c(struct interrupt_frame* frame) { kprintf(12, 0, "Reserved 22"); while(1); }
void isr_23_c(struct interrupt_frame* frame) { kprintf(12, 0, "Reserved 23"); while(1); }
void isr_24_c(struct interrupt_frame* frame) { kprintf(12, 0, "Reserved 24"); while(1); }
void isr_25_c(struct interrupt_frame* frame) { kprintf(12, 0, "Reserved 25"); while(1); }
void isr_26_c(struct interrupt_frame* frame) { kprintf(12, 0, "Reserved 26"); while(1); }
void isr_27_c(struct interrupt_frame* frame) { kprintf(12, 0, "Reserved 27"); while(1); }
void isr_28_c(struct interrupt_frame* frame) { kprintf(12, 0, "Reserved 28"); while(1); }
void isr_29_c(struct interrupt_frame* frame) { kprintf(12, 0, "Reserved 29"); while(1); }
void isr_30_c(struct interrupt_frame* frame) { kprintf(12, 0, "Reserved 30"); while(1); }
void isr_31_c(struct interrupt_frame* frame) { kprintf(12, 0, "Reserved 31"); while(1); }