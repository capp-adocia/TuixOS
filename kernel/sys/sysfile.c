/* kernel/sys/sysfile.c */

#include "stddef.h"
#include <Tuix/panic.h>
#include <Tuix/sysfile.h>
#include <Tuix/serial.h>
#include <Tuix/file.h>
#include <string.h>
#include <Tuix/process.h>

static char* extract_filename(const char* path, char* file_name)
{
    UNUSED(path);
    // if (path == NULL || file_name == NULL)
    //     PANIC("参数为空");
    //
    // // 检查是否以 '/' 开头
    // if (path[0] != '/')
    //     PANIC("路径需要以/开始");
    //
    // // 检查长度（至少2个字符："/" + 文件名）
    // size_t total_len = strlen(path);
    // if (total_len < 2)
    //     PANIC("路径太短");
    //
    // // 检查文件名部分长度
    // size_t name_len = total_len - 1;
    // if (name_len >= MAX_NAME)
    //     PANIC("文件名称过长");
    //
    // // 检查是否包含其他 '/'（禁止目录）
    // if (strchr(path + 1, '/') != NULL)
    //     PANIC("文件名称中不能包含/");
    //
    // // 拷贝文件名（去掉开头的 '/'）
    // strncpy(file_name, path + 1, name_len);
    // file_name[name_len] = '\0';  // 确保终止
    //
    // // 检查文件名有效性
    // if (file_name[0] == '\0')
    //     PANIC("文件名为空");
    //
    // // 检查非法字符
    // for (size_t i = 0; i < name_len; i++)
    // {
    //     if (file_name[i] == '\0' || file_name[i] == '\n' || 
    //         file_name[i] == '\t' || file_name[i] == ' ')
    //         PANIC("文件名包含非法字符");
    // }
    //
    return file_name;
}

int sys_write(int fd, const void* buf, size_t count)
{
    UNUSED(buf);
    // if (fd < 0 || fd >= MAX_FILES)
    //     return -1;

    if (cpus[cpu_id].proc->ofile[fd]->ref == 0)
        return -1;

    serial_printf("有效文件，文件信息:\n");
    return count;
}

int sys_open(const char* path, int mode)
{
    UNUSED(path);
    int fd = -1; // 文件描述符
    char file_name[20];
    // 先提取路径名称，看看符不符合正常的路径
    extract_filename(path, file_name);
    // serial_printf("文件名为:%s 大小为:%d\n", file_name, strlen(file_name));
    // 从files中查找文件或者创建文件
    // 如果mode是创建模式打开则需要先创造一个文件
    if(mode & O_CREAT)
    {
        // // 从文件数组中找到一个未用过的文件，取出后标记为用过
        // for(int i = 0;i < MAX_FILES;i++)
        // {
        //     if(files[i].used == 0)
        //     {
        //         files[i].used = 1;
        //         files[i].size = 0; // 文件大小
        //         strncpy(files[i].name, file_name, strlen(file_name));
        //         fd = i;
        //         break;
        //     }
        // }
        // return (fd >= 0) ? fd : -1;
    }
    else
    {
        // 根据路径在files表中查找到对应数据
        // for(int i = 0;i < MAX_FILES;i++)
        // {
        //     // if(strcmp(files[i].name, ))
        // }
    }

    return fd;
}

int sys_read(int fd, void* buf, size_t count)
{
    UNUSED3(fd, buf, count);

    return -1;
}
