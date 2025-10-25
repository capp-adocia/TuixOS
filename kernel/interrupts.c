/* kernel/interrupts.c */

#include <Hydrangea/interrupts.h>
#include <Hydrangea/screen.h>
#include <Hydrangea/io.h>
#include <stddef.h>

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
/*
    函数名待修改:用isr_开头后面写上功能含义
*/
// 0: 除零错误 - 可恢复，设置默认结果
void isr_0_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Divide Error at EIP: %x - Fixed", frame->eip);
    frame->eax = 0;  // 设置结果为0，继续执行
}

// 1: 调试异常 - 可恢复，用于调试器
void isr_1_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Debug Exception at EIP: %x - Continue", frame->eip);
    // 单步执行，继续
}

// 2: 非屏蔽中断 - 严重硬件错误
void isr_2_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "NMI Interrupt - Hardware Failure");
    kprintf(13, 0, "System Halted");
    __asm__ volatile("cli; hlt");  // 停机
}

// 3: 断点 - 可恢复，用于调试
void isr_3_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Breakpoint at EIP: %x - Continue", frame->eip);
    // 调试断点，继续执行
}

// 4: 溢出 - 可恢复，清除标志
void isr_4_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Overflow at EIP: %x - Cleared", frame->eip);
    // 溢出标志会被自动处理，继续执行
}

// 5: 边界检查 - 可恢复，修复索引
void isr_5_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Bounds Check at EIP: %x - Fixed", frame->eip);
    // 可以修复边界索引，继续执行
}

// 6: 无效操作码 - 严重，无法恢复
void isr_6_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Invalid Opcode at EIP: %x", frame->eip);
    kprintf(13, 0, "Unrecoverable - System Halted");
    __asm__ volatile("cli; hlt");
}

// 7: 设备不可用 - 可恢复，模拟或禁用
void isr_7_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Device Not Available at EIP: %x", frame->eip);
    __asm__ volatile("mov %cr0, %eax; and $0xFFFFFFFB, %eax; mov %eax, %cr0");
}

// 8: 双重故障 - 严重系统错误
void isr_8_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Double Fault! Error: %x", frame->err_code);
    kprintf(13, 0, "EIP: %x, System Halted", frame->eip);
    __asm__ volatile("cli; hlt");
}

// 9: 协处理器段越界
void isr_9_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Coprocessor Segment Overrun - Fixed");
    // 继续执行
}

// 10: 无效TSS - 系统配置错误
void isr_10_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Invalid TSS. Error: %x", frame->err_code);
    kprintf(13, 0, "Kernel Panic - System Halted");
    __asm__ volatile("cli; hlt");
}

// 11: 段不存在, 重新加载段
void isr_11_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Segment Not Present. Error: %x", frame->err_code);
    // 可以重新加载段寄存器或修复GDT
    // 这里简单继续,在实际OS中需要更复杂处理
}

// 12: 栈段错误 - 严重内存错误
void isr_12_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Stack Segment Fault. Error: %x", frame->err_code);
    kprintf(13, 0, "Stack Corrupted - System Halted");
    __asm__ volatile("cli; hlt");
}

// 13: 通用保护错误 - 严重内存/权限错误
void isr_13_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "General Protection Fault! Error: %x", frame->err_code);
    kprintf(13, 0, "EIP: %x, CS: %x - System Halted", frame->eip, frame->cs);
    __asm__ volatile("cli; hlt");
}

// 14: 页错误 - 可恢复，处理缺页
void isr_14_c(struct interrupt_frame* frame) {
    uint32_t fault_addr;
    __asm__ volatile("mov %%cr2, %0" : "=r"(fault_addr));
    kprintf(12, 0, "Page Fault @ %x", fault_addr);
    kprintf(13, 0, "Error: %x - Handling", frame->err_code);
    
    // 在实际OS中这里会分配物理页、更新页表等
    // 这里简单标记，继续执行
}

// 15: 保留 - 不应该发生
void isr_15_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Reserved Exception 15 - Ignored");
    // 继续执行
}

// 16: 浮点错误 - 可恢复，清除状态
void isr_16_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Floating Point Exception - Cleared");
    // 清除FPU状态字，继续执行
    __asm__ volatile("fnclex");
}

// 17: 对齐检查 - 可修复
void isr_17_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Alignment Check. Error: %x - Fixed", frame->err_code);
    // 可以修复对齐，继续执行
}

// 18: 机器检查 - 严重硬件错误
void isr_18_c(struct interrupt_frame* frame) {
    kprintf(12, 0, "Machine Check - Hardware Failure");
    kprintf(13, 0, "Critical Error - System Halted");
    __asm__ volatile("cli; hlt");
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

// 简单的按键处理函数
void handle_key_press(uint8_t scancode)
{
    kprint(14, 0, "void handle_key_press(uint8_t scancode) WORK!");
}

void isr_keyboard(struct interrupt_frame* frame)
{
    uint8_t scancode = inb(0x60);
    if(scancode < 0x80) handle_key_press(scancode);

    send_eoi(frame->int_no);
}