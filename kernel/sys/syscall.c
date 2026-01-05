/* kernel/sys/syscall.c */

#include <Tuix/syscall.h>
#include <Tuix/sysconf.h>
#include <Tuix/serial.h>
#include <Tuix/process.h>
#include <Tuix/sysfile.h>

void syscall_handlers(struct trap_frame* frame)
{
    int syscall_num = frame->eax;  // 系统调用号
    int arg1 = frame->ebx;         // 第一个参数
    int arg2 = frame->ecx;         // 第二个参数
    int arg3 = frame->edx;         // 第三个参数
    serial_printf("系统调用号为: %d\n", syscall_num);
    switch(syscall_num)
    {
        case SYS_EXIT:
            {
                // 退出后释放进程的空间，执行一次调度，切换到下一个进程
                cpus[cpu_id].proc->state = ZOMBIE;
                sched();
                break;
            }
        case SYS_WRITE:
            {
                sys_write(arg1, (char*)arg2, arg3);
                break;
            }
    }
}
