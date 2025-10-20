/* include/string.h */

#ifndef I_STRING_H
#define I_STRING_H

/**
 * 计算字符串长度
 * @param str 要计算长度的字符串
 */
static __attribute__((always_inline)) int strlen(const char* str)
{
    int len = 0;
    while(str[len] != '\0') len++;
    return len;
}

/**
 * 整型转为字符串
 * @param num 待转换的整数
 * @param buffer 输出字符串
 */
static __attribute__((always_inline)) void int_to_str(int num, char* buffer)
{
    if (num < 10) {
        buffer[0] = '0' + num;
        buffer[1] = '\0';
    } else if (num < 100) {
        buffer[0] = '0' + (num / 10);
        buffer[1] = '0' + (num % 10);
        buffer[2] = '\0';
    } else {
        // 处理三位数
        buffer[0] = '0' + (num / 100);
        buffer[1] = '0' + ((num / 10) % 10);
        buffer[2] = '0' + (num % 10);
        buffer[3] = '\0';
    }
}


#endif