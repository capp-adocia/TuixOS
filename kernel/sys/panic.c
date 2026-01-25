/* kernel/sys/panic.c */

#include <arch/x86.h>
#include <Tuix/panic.h>
#include <Tuix/serial.h>

void panic(const char* file, int line, const char* msg)
{
    serial_printf("\n*** PANIC at %s:%d: %s ***\n", file, line, msg);
    while(1) { cli(); hlt(); };
}
