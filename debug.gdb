# debug.gdb - GDB 调试脚本
set architecture i8086
target remote localhost:1234
break *0x7c00
add-symbol-file stage1.debug.o 0x7c00
add-symbol-file stage2.debug.o 0x7e00
break prepare_pmode
echo 
echo === HydrangeaOS Debug Session ===
echo Commands:
echo   continue  - 继续执行
echo   stepi     - 单步执行
echo   info registers - 查看寄存器
echo   x/10i $pc - 查看当前指令
echo   quit      - 退出 GDB
echo 
continue
