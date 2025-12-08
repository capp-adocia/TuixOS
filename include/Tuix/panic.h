/* include/Tuix/panic.h */

#ifndef I_T_PANIC_H
#define I_T_PANIC_H

#define PANIC(msg) panic(__FILE__, __LINE__, msg)

/**
 * panic用于指出重大错误
 * @param file 文件名
 * @param line 行号
 * @param msg 错误消息
 */
void panic(const char* file, int line, const char* msg);
#endif
