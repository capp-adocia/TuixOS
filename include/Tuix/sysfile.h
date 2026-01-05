/* include/Tuix/sysfile.h */

#ifndef I_T_SYSFILE_H
#define I_T_SYSFILE_H

#include <stddef.h>

/**
 * 系统调用：sys_write 写入文件
 * @parma fd 文件描述符
 * @parma buf 数据指针
 * @parma n 数据长度
 * @return 返回写入的字节数
 */
int sys_write(int fd, char* buf, int n);

#endif
