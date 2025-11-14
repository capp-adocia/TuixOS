/* driver/timer.c */

#include <Silan/timer.h>
#include <Silan/io.h>
#include <Silan/serial.h>
#include <Silan/pic.h>

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

void timer_handler(struct interrupt_frame* frame)
{
    timer_ticks++;
    
    if(timer_ticks % 10 == 0)
        serial_printf("Timer tick: %d\n", timer_ticks);

    send_eoi(frame->int_no);
}