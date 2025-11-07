/* kernel/serial.c */

#include <Silan/serial.h>

void init_serial()
{
    outb(0x3F8 + 1, 0x00);    // 禁用所有中断
    outb(0x3F8 + 3, 0x80);    // 启用DLAB（设置波特率）
    outb(0x3F8 + 0, 0x03);    // 设置波特率低位 38400 baud
    outb(0x3F8 + 1, 0x00);    // 设置波特率高位
    outb(0x3F8 + 3, 0x03);    // 8位数据，无校验，1停止位
    outb(0x3F8 + 2, 0xC7);    // 启用FIFO，清空，14字节阈值
    outb(0x3F8 + 4, 0x0B);    // 启用IRQ，设置RTS/DSR
}

void serial_putchar(char c)
{
    while ((inb(0x3F8 + 5) & 0x20) == 0);
    outb(0x3F8, c);    
}

void serial_printf(const char* str)
{
    while (*str) serial_putchar(*str++);
}