/* include/screen.h */

#ifndef I_SCREEN_H
#define I_SCREEN_H

#include <types.h>

/**
 * 在指定位置输出一个字符
 * @param c 要输出的字符
 * @param row 行位置 (0-24)
 * @param col 列位置 (0-79)
 */
static __attribute__((always_inline)) void put_char(const char c, int row, int col)
{
    char* video = (char*)0xB8000 + (row * 80 + col) * 2;
    video[0] = c;
    video[1] = 0x0E;
}

/**
 * 在指定位置输出一个字符串
 * @param str 要输出的字符串
 * @param row 行位置 (0-24)
 * @param col 列位置 (0-79)
 */
static __attribute__((always_inline)) void kprint(const char* str, int row, int col)
{
    for(int i = 0; str[i] != '\0'; i++)
    {
        put_char(str[i], row, col + i);
    }
}

/**
 * 清屏函数
 */
void static __attribute__((always_inline)) clear_screen(void)
{
    char* video = (char*)0xB8000;
    for(int i = 0;i < 80 * 25 * 2;i += 2)
    {
        video[i] = ' ';
        video[i + 1] = 0x07;
    }
}

#endif