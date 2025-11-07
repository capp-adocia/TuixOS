/* kernel/serial.c */

#include <Silan/serial.h>
#include <stdarg.h>

static void serial_print_int(int num, int base);
static void serial_print_hex(unsigned int num);

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

// 串口格式化输出函数
void serial_printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    
    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 's': { // 字符串
                    char* str = va_arg(args, char*);
                    while (*str) serial_putchar(*str++);
                    break;
                }
                case 'c': { // 字符
                    char ch = (char)va_arg(args, int);
                    serial_putchar(ch);
                    break;
                }
                case 'd': { // 十进制整数
                    int num = va_arg(args, int);
                    serial_print_int(num, 10);
                    break;
                }
                case 'x': { // 十六进制
                    unsigned int num = va_arg(args, unsigned int);
                    serial_print_hex(num);
                    break;
                }
                case 'p': { // 指针地址
                    void* ptr = va_arg(args, void*);
                    serial_putchar('0');
                    serial_putchar('x');
                    serial_print_hex((uint32_t)ptr);
                    break;
                }
                case '%': { // 转义百分号
                    serial_putchar('%');
                    break;
                }
                default:
                    serial_putchar('%');
                    serial_putchar(*format);
                    break;
            }
        } else {
            serial_putchar(*format);
        }
        format++;
    }
    
    va_end(args);
}

// 打印十进制整数
static void serial_print_int(int num, int base)
{
    char buffer[32];
    char* ptr = buffer;
    int is_negative = 0;
    
    if (num == 0) {
        serial_putchar('0');
        return;
    }
    
    if (num < 0 && base == 10) {
        is_negative = 1;
        num = -num;
    }
    
    // 从低位到高位转换数字
    while (num > 0) {
        int digit = num % base;
        *ptr++ = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        num /= base;
    }
    
    // 添加负号
    if (is_negative) {
        *ptr++ = '-';
    }
    
    // 反向输出（因为转换时是逆序的）
    while (ptr > buffer) {
        serial_putchar(*--ptr);
    }
}

// 打印十六进制数
static void serial_print_hex(unsigned int num)
{
    serial_putchar('0');
    serial_putchar('x');
    
    if (num == 0) {
        serial_putchar('0');
        return;
    }
    
    // 从高位到低位转换
    char buffer[9];
    for (int i = 7; i >= 0; i--) {
        int digit = (num >> (i * 4)) & 0xF;
        buffer[7-i] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
    }
    
    // 跳过前导零
    int start = 0;
    while (start < 7 && buffer[start] == '0') {
        start++;
    }
    
    // 输出有效数字
    for (int i = start; i < 8; i++) {
        serial_putchar(buffer[i]);
    }
}