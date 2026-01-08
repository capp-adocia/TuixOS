/* include/string.h */

#ifndef I_STRING_H
#define I_STRING_H

#include <stddef.h>

/**
 * 用指定值填充内存区域
 * @param dst 目标内存起始地址
 * @param val 要填充的值（0-255）
 * @param count 要填充的字节数
 * @return 返回dst（便于链式调用）
 */
void* memset(void* dst, int val, size_t count);

/**
 * 内存复制（不处理重叠）
 * @param dst 目标地址
 * @param src 源地址  
 * @param count 字节数
 * @return 目标指针
 */
void* memcpy(void* dst, const void* src, size_t count);

/**
 * 内存复制（处理重叠区域，可以保证安全）
 * @param dst 目标地址
 * @param src 源地址
 * @param count 字节数
 * @return 目标指针
 */
void* memmove(void* dst, const void* src, size_t count);

/**
 * 内存比较
 * @param ptr1 内存块1
 * @param ptr2 内存块2  
 * @param count 比较字节数
 * @return 当ptr1 < ptr2时返回-1, 当ptr1 > ptr2时返回1，相等返回0
 */
int memcmp(const void* ptr1, const void* ptr2, size_t count);

/**
 * 计算字符串长度
 * @param str 要计算长度的字符串
 */
inline size_t strlen(const char* str)
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
 * 字符串拷贝
 * @param dst 目标字符串
 * @param src 源字符串
 * @return 返回目标字符串拷贝
 */
char* strcpy(char* dst, const char* src);

/**
 * 安全字符串拷贝（带长度限制）
 * @param dst 目标字符串
 * @param src 源字符串
 * @param n   拷贝字节数
 * @return 返回目标字符串拷贝
 */
char* strncpy(char* dst, const char* src, size_t n);

/**
 * 字符串连接
 * @param dst 目标字符串
 * @param src 源字符串
 * @return 返回目标字符串拷贝
 */
char* strcat(char* dst, const char* src);

/**
 * 字符串连接(指定数量n)
 * @param dst 目标字符串
 * @param src 源字符串
 * @param n   拷贝字符数
 * @return 返回目标字符串拷贝
 */
char* strncat(char* dst, const char* src, size_t n);

/**
 * 字符串比较
 * @param s1 源字符串1
 * @param s2 源字符串2
 * @return 返回比较结果: val = 0 => "相等" or val < 0 => "不相等"
 */
int strcmp(const char* s1, const char* s2);

/**
 * 字符串比较(比较前n个)
 * @param s1 源字符串1
 * @param s2 源字符串2
 * @param n  按前n个字符比较
 * @return 返回比较结果
 *         val = 0 => "前n个相等"
 *         val < 0 => "前n个不相等"
 */
int strncmp(const char* s1, const char* s2, size_t n);


/**
 * 字符查找(从前向后查找字符串中第一个出现的字符指针)
 * @param s 目标字符串
 * @param c 待查找字符
 * @return 返回该字符在字符串中指针
 */
char* strchr(const char* s, int c);

/**
 * 字符查找(从后向前查找字符串中最后一个出现的字符指针)
 * @param s 目标字符串
 * @param c 待查找字符
 * @return 返回该字符在字符串中指针
 */
char* strrchr(const char* s, int c);

/**
 * @param haystack 被搜索的主字符串
 * @param needle 要查找的子字符串
 * @return 如果找到，返回指向 haystack 中子串首次出现位置的指针；
 *         如果没找到，返回 NULL；
 *         如果 needle 是空字符串，返回 haystack
 */
char* strstr(const char* haystack, const char* needle);

#endif
