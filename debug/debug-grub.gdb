# debug-grub.gdb

set architecture i386

target remote localhost:1234

echo "=== 加载C内核调试信息 ===\n"
file out/Tuix.kernel

echo "=== 设置断点 ===\n"
break kernel_main

echo "=== 开始执行 ===\n"
continue