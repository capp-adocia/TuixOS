/* include/stdarg.h */

#ifndef I_STDARG_H
#define I_STDARG_H

typedef char* va_list;
// 可变参数
#define va_start(ap, last) (ap = (va_list)&last + sizeof(last))
#define va_arg(ap, type) (*(type*)((ap += sizeof(type)) - sizeof(type)))
#define va_end(ap) (ap = (va_list)0)

#endif