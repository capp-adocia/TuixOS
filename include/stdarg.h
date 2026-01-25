/* include/stdarg.h */

#ifndef I_STDARG_H
#define I_STDARG_H

typedef char* va_list;

/* 所有参数按4字节对齐（x86兼容） */
#define va_start(ap, last) (ap = ((va_list)&(last)) + 4)
#define va_arg(ap, type) (*(type*)((ap += 4) - 4))
#define va_end(ap) (ap = (va_list)0)

#endif
