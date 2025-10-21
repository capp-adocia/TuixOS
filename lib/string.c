/* lib/string.c */

#include <string.h>

void int_to_str(int num, char* buffer)
{
if (num == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }
    int i = 0;
    int is_negative = 0;
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }
    // 反向存储数字
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    if (is_negative) {
        buffer[i++] = '-';
    }
    buffer[i] = '\0';
    // 反转字符串
    reverse_str(buffer);
}

void reverse_str(char* str)
{
    int len = 0;
    while (str[len] != '\0') len++;
    
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
}