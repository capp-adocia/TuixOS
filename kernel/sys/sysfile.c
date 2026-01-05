/* kernel/sys/sysfile.c */

#include <Tuix/sysfile.h>
#include <Tuix/serial.h>

int sys_write(int fd, char* buf, int n)
{
    if(fd == 1)
    {
        for (int i = 0; i < n; i++)
        {
            serial_putchar(buf[i]);
        }
        return n;
    }
    return -1;
}
