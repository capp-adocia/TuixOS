/* include/Tuix/sysfile.h */

#ifndef I_T_SYSFILE_H
#define I_T_SYSFILE_H

#include <stddef.h>

/**
 * 写入文件
 * @parma fd 文件描述符
 * @parma buf 数据指针
 * @parma count 数据长度
 * @return 返回写入的字节数
 */
int sys_write(int fd, const void* buf, size_t count);

/**
 * 打开文件
 * @parma path 文件的打开路径
 * @parma mode 以哪种模式打开
 * @return 返回文件描述符fd
 */
int sys_open(const char* path, int mode);

/**
 * 读取文件
 * @parma fd 文件描述符
 * @parma buf 数据指针
 * @parma count 读取字节数
 * @return 实际读取字节数
 */
int sys_read(int fd, void* buf, size_t count);

#endif
