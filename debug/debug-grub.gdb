# debug-grub.gdb

set architecture i386

target remote localhost:1234

echo "=== 加载C内核调试信息 ===\n"
file out/Tuix.kernel

echo "=== 设置断点 ===\n"
# break init_uvm
break fork_ret

# 汇编形式
set disassembly-flavor intel
# TUI 布局
tui enable
layout src
layout regs
focus src

echo "=== 开始执行 ===\n"
continue
