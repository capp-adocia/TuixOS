/* include/string.h */

#ifndef I_STRING_H
#define I_STRING_H

#include <types.h>

/**
 * 计算字符串长度
 * @param str 要计算长度的字符串
 */
static __attribute__((always_inline)) size_t strlen(const char* str)
{
    int len = 0;
    while(str[len] != '\0') len++;
    return len;
}

/**
 * 整型转为字符串
 * @param num 待转换的整数
 * @param buffer 输出字符串
 */
void int_to_str(int num, char* buffer);
/**
 * 字符串反转
 * @param str 待转换的字符串
 */
void reverse_str(char* str);

/**
 * 字符串复制
 * @param dst 目标缓冲区
 * @param src 源字符串
 */
static __attribute__((always_inline)) char* strcpy(char* dst, const char* src);

/**
 * 安全字符串复制（带长度限制）
 */
static __attribute__((always_inline)) char* strncpy(char* dst, const char* src, size_t n);

/**
 * 字符串连接
 */
static __attribute__((always_inline)) char* strcat(char* dst, const char* src);

static __attribute__((always_inline)) char* strncat(char* dst, const char* src, size_t n);


/**
 * 字符串比较
 */
static __attribute__((always_inline)) int strcmp(const char* s1, const char* s2);

static __attribute__((always_inline)) int strncmp(const char* s1, const char* s2, size_t n);

#endif