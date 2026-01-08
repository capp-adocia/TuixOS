/* include/Tuix/screen.h */

#ifndef I_T_SCREEN_H
#define I_T_SCREEN_H

#include <stddef.h>
#include <stdarg.h>

/**
 * 在指定位置输出一个字符
 * @param c 要输出的字符
 * @param row 行位置 (0-24)
 * @param col 列位置 (0-79)
 */
void put_char(const char c, int row, int col);

/**
 * 在指定位置格式化输出
 * @param row 行位置 (0-24)
 * @param col 列位置 (0-79)
 * @param fmt 可变参数...
 */
void kprintf(int row, int col, const char* fmt, ...);

/**
 * 在指定位置输出一个字符串
 * @param row 行位置 (0-24)
 * @param col 列位置 (0-79)
 * @param str 要输出的字符串
 */
void kprint(int row, int col, const char* str);

/**
 * 清屏函数
 */
void clear_screen(void);

#endif
