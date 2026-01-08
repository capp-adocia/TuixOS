/* lib/string.c */

#include <string.h>

void* memset(void* dst, int val, size_t count)
{
    if (dst == NULL) 
        return NULL;
    
    char* temp = (char*)dst;
    for (size_t i = 0; i < count; i++)
        *temp++ = (char)val;
    
    return dst;
}

void* memcpy(void* dst, const void* src, size_t count)
{
    if (dst == NULL || src == NULL)
        return NULL;
    
    if (count == 0)
        return dst;
    
    char* dst_t = (char*)dst;
    const char* src_t = (const char*)src;
    
    while (count--) *dst_t++ = *src_t++;
    return dst;
}

void* memmove(void* dst, const void* src, size_t count)
{
    if (dst == NULL || src == NULL)
        return NULL;
    
    if (count == 0 || dst == src)
        return dst;
    
    char* d = (char*)dst;
    const char* s = (const char*)src;
    // 目标在源前面，正向拷贝
    if (d < s) while (count--) *d++ = *s++;
    else
    {
        // 目标在源后面，反向拷贝从末尾开始
        d += count;
        s += count;
        while (count--)
            *--d = *--s;
    }
    return dst;
}

int memcmp(const void* ptr1, const void* ptr2, size_t count)
{
    if (ptr1 == NULL || ptr2 == NULL)
        return -1;  // 或者返回不相等
    
    // 如果count为0，认为相等
    if (count == 0)
        return 0;
    
    const unsigned char* p1 = (const unsigned char*)ptr1;
    const unsigned char* p2 = (const unsigned char*)ptr2;
    
    while (count-- > 0){
        if (*p1 != *p2) return (*p1 < *p2) ? -1 : 1;
        p1++;
        p2++;
    }
    return 0;
}

void int_to_str(int num, char* buffer)
{
    if (num == 0)
    {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }
    int i = 0;
    int is_negative = 0;
    if (num < 0)
    {
        is_negative = 1;
        num = -num;
    }
    // 反向存储数字
    while (num > 0)
    {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    if (is_negative) 
    {
        buffer[i++] = '-';
    }
    buffer[i] = '\0';
    // 反转字符串
    reverse_str(buffer);
}

void reverse_str(char* str)
{
    int len = 0;
    while (str[len] != '\0') len++;
    
    for (int i = 0; i < len / 2; i++) 
    {
        char temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
}

char* strcpy(char* dst, const char* src)
{
    char *os;

    os = dst;
    while((*dst++ = *src++) != 0);

    return os;
}

char* strncpy(char* dst, const char* src, size_t n)
{
    char *d = dst;
    const char *s = src;

    while (n > 0 && *s != '\0')
    {
        *d++ = *s++;
        n--;
    }

    while (n > 0)
    {
        *d++ = '\0';
        n--;
    }

    return dst;
}

char* strcat(char* dst, const char* src)
{
    char *os = dst;
    while (*dst != '\0') dst++;
    while ((*dst++ = *src++) != '\0');

    return os;
}

char* strncat(char* dst, const char* src, size_t n)
{
    char *os = dst;
    while (*dst != '\0') dst++;

    while (n > 0 && (*dst++ = *src++) != '\0') n--;
    // 确保以 '\0' 结尾
    *dst = '\0';

    return os;
}

int strcmp(const char* s1, const char* s2)
{
    while (*s1 != '\0' && *s1 == *s2)
    {
        s1++;s2++;
    }

    return *(uint8_t*)s1 - *(uint8_t*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n)
{
    if (n == 0)
        return 0;

    while (n > 1 && *s1 != '\0' && *s1 == *s2)
    {
        s1++;
        s2++;
        n--;
    }
    
    return *(uint8_t*)s1 - *(uint8_t*)s2;
}

char* strchr(const char* s, int c)
{
    while (*s != '\0')
    {
        if (*s == (char)c)
            return (char*)s;
        s++;
    }

    return (*s == (char)c) ? (char*)s : NULL;
}

char* strrchr(const char* s, int c)
{
    const char *last = NULL;
    while (*s != '\0')
    {
        if (*s == (char)c)
            last = s;
        s++;
    }

    return (*s == (char)c) ? (char*)s : (char*)last;
}

char* strstr(const char* haystack, const char* needle)
{
    if (*needle == '\0')
        return (char*)haystack;

    for (; *haystack != '\0'; haystack++)
    {
        const char *h = haystack;
        const char *n = needle;

        while (*h != '\0' && *n != '\0' && *h == *n)
        {
            h++;
            n++;
        }

        if (*n == '\0')
            return (char*)haystack;
    }

    return NULL;
}
