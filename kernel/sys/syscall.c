/* kernel/sys/syscall.c */

#include <Tuix/syscall.h>
#include <Tuix/sysconf.h>
#include <Tuix/serial.h>
#include <Tuix/process.h>

void syscall_handlers(struct trap_frame* frame)
{
    serial_printf("hello, syscall-frame->eax: %d\n", frame->eax);
    switch(frame->eax)
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

                break;
            }
    }
}
