/* include/Tuix/panic.h */

#ifndef I_T_PANIC_H
#define I_T_PANIC_H

#include <arch/x86.h>
#include <Tuix/serial.h>

#define PANIC(msg) panic(__FILE__, __LINE__, msg)

/**
 * panic用于指出重大错误
 * @param file 文件名
 * @param line 行号
 * @param msg 错误消息
 */
static inline void panic(const char* file, int line, const char* msg)
{
    serial_printf("\n***\n PANIC at %s:%d => %s \n***\n", file, line, msg);
    while(1) { cli(); hlt(); };
}

#endif
