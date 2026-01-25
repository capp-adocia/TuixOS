/* tools/stat.h */

#ifndef T_STAT_H
#define T_STAT_H

#include "type.h"

#define T_DIR  1   // directory
#define T_FILE 2   // file
#define T_DEV  3   // device

/* stat结构体原本就存在POSIX标准,所以这里需要更换名称 */
struct t_stat
{
    short type;     // file type
    int dev;        // device
    uint32_t ino;     // inode num
    short nlink;    // file link num
    uint32_t size;    // file size
};

#endif
