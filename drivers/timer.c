/* driver/timer.c */

#include "Tuix/ptrace.h"
#include <Tuix/timer.h>
#include <Tuix/io.h>
#include <Tuix/serial.h>
#include <Tuix/pic.h>
#include <Tuix/process.h>

volatile uint32_t timer_ticks = 0;

void init_timer(uint32_t frequency)
{
    if(frequency == 0)
    {
        serial_printf("frequency cannot be zero!\n");
        return;
    }
    uint32_t divisor = 1193180 / frequency;
    // 限制分频值在有效范围内
    if(divisor > 65535)
    {
        divisor = 65535;
        serial_printf("Warning: frequency too low, using minimum frequency\n");
    } 
    else if(divisor < 1)
    {
        divisor = 1;
        serial_printf("Warning: frequency too high, using maximum frequency\n");
    }
    
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
    
    serial_printf("Timer initialized with frequency: %d Hz\n", frequency);
}

// 每10ms强制切换进程
void timer_handler(struct pt_regs* regs)
{
    uint32_t current_esp;
    __asm__ volatile("mov %%esp, %0" : "=r"(current_esp));
    
    serial_printf("current: %x \n", current_esp);

    timer_ticks++;
    
    /* 执行调度，选择一个新进程 */
    schedule();

    /* 此后，当前中断帧里保存的就是新pcb的数据 */
    send_eoi(regs->int_no);
}
