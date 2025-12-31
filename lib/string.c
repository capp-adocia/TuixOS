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
