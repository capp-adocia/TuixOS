/* include/errno.h */

#ifndef I_ERRNO_H
#define I_ERRNO_H

/* 通用错误码 */
#define EPERM        1  /* 操作不允许 */
#define ENOENT       2  /* 文件或目录不存在 */
#define EIO          5  /* I/O错误 */
#define ENOMEM      12  /* 内存不足 */
#define EFAULT      14  /* 错误地址 */
#define EBUSY       16  /* 设备或资源忙 */
#define EEXIST      17  /* 文件已存在 */
#define EINVAL      22  /* 无效参数 */
#define ENOSPC      28  /* 设备无空间 */
#define ERANGE      34  /* 结果超出范围 */

/* 内存相关错误码 */
#define EMMBADADDR  100 /* 内存错误地址 */
#define EMMDOUBLE   101 /* 内存重复释放 */
#define EMMCORRUPT  102 /* 内存结构损坏 */

/* 文件系统错误码 */
#define EFSIO       200 /* 文件系统I/O错误 */
#define EFSCORRUPT  201 /* 文件系统损坏 */

/* 进程相关错误码 */
#define EPROCNOTFOUND 300 /* 进程未找到 */

#endif