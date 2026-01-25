/* include/Tuix/serial.h */

#ifndef I_T_SERIAL_H
#define I_T_SERIAL_H

#include <arch/x86.h>

/**
 * 初始化串口
 */
void init_serial(void);

/**
 * 串口输出字符
 * @param c 字符
 */
void serial_putchar(const char c);

/**
 * 串口格式化输出字符串
 */
void serial_printf(const char* format, ...);

#endif
