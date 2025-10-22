# debug.gdb

set architecture i8086

target remote localhost:1234

echo "=== 加载调试符号 ===\n"
add-symbol-file out/stage1d.elf 0x7c00
add-symbol-file out/stage2d.elf 0x7e00

echo "=== 加载C内核调试信息 ===\n"
file out/kernel.elf                    
add-symbol-file out/kernel.elf 0x9000  

echo "=== 设置断点 ===\n"
break *0x7c00
break protected_mode_entry
break kernel_main

echo "=== 开始执行 ===\n"
continue
continue
continue