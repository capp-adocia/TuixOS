/* include/arch/x86.h */

#ifndef I_ARCH_X86_H
#define I_ARCH_X86_H

#include <stddef.h>

/* 端口输出字节 */
static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

/* 端口输入字节 */
static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* 端口输出字 (16位) */
static inline void outw(uint16_t port, uint16_t value)
{
    __asm__ volatile("outw %0, %1" : : "a"(value), "Nd"(port));
}

/* 端口输入字 (16位) */
static inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    __asm__ volatile("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* 端口输出双字 (32位) */
static inline void outl(uint16_t port, uint32_t value)
{
    __asm__ volatile("outl %0, %1" : : "a"(value), "Nd"(port));
}

/* 端口输入双字 (32位) */
static inline uint32_t inl(uint16_t port)
{
    uint32_t ret;
    __asm__ volatile("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* 延迟一小段时间 */
static inline void io_wait(void)
{
    outb(0x80, 0);
}

/* 关中断 */
static inline void cli(void)
{
    __asm__ volatile("cli");
}

/* 开中断 */
static inline void sti(void)
{
    __asm__ volatile("sti");
}

/* 停机 */
static inline void hlt(void)
{
    __asm__ volatile("hlt");
}

#endif
