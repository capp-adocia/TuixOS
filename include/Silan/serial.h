/* include/Silan/serial.h */

#ifndef I_H_SERIAL_H
#define I_H_SERIAL_H

#include <Silan/io.h>

/**
 * 初始化串口
 */
void init_serial();

/**
 * 串口输出字符
 * @param c 字符
 */
void serial_putchar(char c);

/**
 * 串口输出字符串
 * @param str 字符串
 */
void serial_printf(const char* str);

#endif