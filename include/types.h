/* include/types.h */

#ifndef I_TYPES_H
#define I_TYPES_H

/* 基础整数类型 */
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;  
typedef unsigned int   uint32_t;
typedef unsigned long long uint64_t;

typedef signed char    int8_t;
typedef signed short   int16_t;
typedef signed int     int32_t;
typedef signed long long int64_t;

/* 大小类型 */
typedef unsigned int   size_t;
typedef signed int     ssize_t;

/* 布尔类型 */
typedef int bool;
#define true 1
#define false 0

/* 其他类型 */
typedef uint32_t phys_addr_t;  // 物理地址
typedef uint32_t virt_addr_t;  // 虚拟地址


#endif