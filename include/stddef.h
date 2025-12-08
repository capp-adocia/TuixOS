/* include/stddef.h */

#ifndef I_STDDEF_H
#define I_STDDEF_H

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
typedef int ptrdiff_t;

/* 布尔类型 */
typedef int bool;
#define true 1
#define false 0

#define NULL ((void*)0)

/* 计算成员函数在结构体中的偏移 */
#define offsetof(type, member) ((size_t)&((type*)0)->member) // 将0作为指针找到member的地址并取地址得到偏移量

/**
 * 根据结构体成员来获取这个结构体的地址
 * @param ptr 成员指针
 * @param type 结构体类型
 * @param member 结构体成员名称
 * @return 结构体地址
 */
#define container_of(ptr, type, member) \
    ((type*)((char*)(ptr) - offsetof(type, member)))


#endif
