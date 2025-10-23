/* driver/screen.c */

#include <Hydrangea/screen.h>

void kprintf(int row, int col, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    
    char buffer[256];
    int pos = 0;
    
    for (int i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
                case 'd': {  // 整数
                    int num = va_arg(args, int);
                    // 数字转字符串
                    if (num == 0) {
                        buffer[pos++] = '0';
                    } else {
                        if (num < 0) {
                            buffer[pos++] = '-';
                            num = -num;
                        }
                        char num_buf[32];
                        int j = 0;
                        while (num > 0) {
                            num_buf[j++] = '0' + (num % 10);
                            num /= 10;
                        }
                        while (j > 0) {
                            buffer[pos++] = num_buf[--j];
                        }
                    }
                    break;
                }
                case 'x': {  // 十六进制整数
                    unsigned int num = va_arg(args, unsigned int);
                    char hex_buf[32];
                    int j = 0;
                    
                    if (num == 0) {
                        buffer[pos++] = '0';
                    } else {
                        // 转换为十六进制字符串（反向）
                        while (num > 0) {
                            int digit = num % 16;
                            if (digit < 10) {
                                hex_buf[j++] = '0' + digit;
                            } else {
                                hex_buf[j++] = 'a' + (digit - 10);
                            }
                            num /= 16;
                        }
                        // 反向输出
                        while (j > 0) {
                            buffer[pos++] = hex_buf[--j];
                        }
                    }
                    break;
                }
                case 's': {  // 字符串
                    char* str = va_arg(args, char*);
                    for (int j = 0; str[j] != '\0'; j++) {
                        buffer[pos++] = str[j];
                    }
                    break;
                }
                case 'c': {  // 字符
                    char ch = (char)va_arg(args, int);
                    buffer[pos++] = ch;
                    break;
                }
                default:
                    buffer[pos++] = fmt[i];
                    break;
            }
        } else {
            buffer[pos++] = fmt[i];
        }
    }
    
    buffer[pos] = '\0';
    va_end(args);
    
    kprint(buffer, row, col);
}