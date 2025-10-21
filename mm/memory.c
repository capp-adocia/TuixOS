/* mm/memory.c */

#ifndef MM_MEMORY_H
#define MM_MEMORY_H

#include <Hydrangea/memory.h>

void memset(void* dst, int val, size_t count)
{
    // int len = count / sizeof(dst[0]);
    // for(int i = 0;i < len;i++)
    // {
    //     dst[i] = val;
    // }
}

void* memcpy(void* dst, const void* src, size_t count)
{

}

void* memmove(void* dst, const void* src, size_t count)
{

}

int memcmp(const void* ptr1, const void* ptr2, size_t count)
{
    
}

#endif