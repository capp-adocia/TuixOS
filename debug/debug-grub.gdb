# debug-grub.gdb

set architecture i386:x86-64

target remote localhost:1234

echo "=== 加载C内核调试信息 ===\n"
file out/Silan.kernel

echo "=== 设置断点 ===\n"
break kernel_main

echo "=== 开始执行 ===\n"
echo "使用 'restart' 命令重启调试，QEMU不会关闭\n
continue