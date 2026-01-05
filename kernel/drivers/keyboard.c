/* kernel/driver/keyboard.c */

#include <Tuix/keyboard.h>
#include <Tuix/screen.h>
#include <Tuix/io.h>
#include <Tuix/serial.h>
#include <Tuix/pic.h>

char scancode_to_char(uint8_t scancode)
{
    switch(scancode)
    {
        /* 数字键 */
        case 0x02: return '1';
        case 0x03: return '2';
        case 0x04: return '3';
        case 0x05: return '4';
        case 0x06: return '5';
        case 0x07: return '6';
        case 0x08: return '7';
        case 0x09: return '8';
        case 0x0A: return '9';
        case 0x0B: return '0';
        /* 字母键 - 第一行 */
        case 0x10: return 'q';
        case 0x11: return 'w';
        case 0x12: return 'e';
        case 0x13: return 'r';
        case 0x14: return 't';
        case 0x15: return 'y';
        case 0x16: return 'u';
        case 0x17: return 'i';
        case 0x18: return 'o';
        case 0x19: return 'p';
        /* 字母键 - 第二行 */
        case 0x1E: return 'a';
        case 0x1F: return 's';
        case 0x20: return 'd';
        case 0x21: return 'f';
        case 0x22: return 'g';
        case 0x23: return 'h';
        case 0x24: return 'j';
        case 0x25: return 'k';
        case 0x26: return 'l';
        /* 字母键 - 第三行 */
        case 0x2C: return 'z';
        case 0x2D: return 'x';
        case 0x2E: return 'c';
        case 0x2F: return 'v';
        case 0x30: return 'b';
        case 0x31: return 'n';
        case 0x32: return 'm';
        /* 符号键 */
        case 0x29: return '`';
        case 0x0C: return '-';
        case 0x0D: return '=';
        case 0x1A: return '[';
        case 0x1B: return ']';
        case 0x2B: return '\\';
        case 0x27: return ';';
        case 0x28: return '\'';
        case 0x33: return ',';
        case 0x34: return '.';
        case 0x35: return '/';
        /* 特殊键 */
        case 0x39: return ' ';  // 空格
        case 0x1C: return '\n'; // 回车
        case 0x0E: return '\b'; // 退格
        case 0x01: return 0x1B; // ESC (ASCII 27)
        
        /* 未处理或释放键 */
        default: return 0;
    }
}

void keyboard_handler(struct trap_frame* frame)
{
    uint8_t scancode = inb(0x60);
    // 按下事件
    if(scancode < 0x80)
    {
        char c = scancode_to_char(scancode);
        if(c)
            serial_printf("按键按下:[%c] 扫描码:%x\n", c, scancode);
        else
            serial_printf("特殊键按下，扫描码:%x\n", scancode);
    }
    // 释放事件
    else
    {
        // uint8_t press_code = scancode - 0x80;
        // serial_printf("按键释放，扫描码:%x\n", press_code);
    }
    
    send_eoi(frame->int_no);
}
