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
                        // 只处理正整数
                        char num_buf[32];
                        int j = 0;
                        while (num > 0) {
                            num_buf[j++] = '0' + (num % 10);
                            num /= 10;
                        }
                        // 反向输出
                        while (j > 0) {
                            buffer[pos++] = num_buf[--j];
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