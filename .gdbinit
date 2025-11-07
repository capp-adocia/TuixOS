# .gdbinit
set confirm off
set pagination off
set disassembly-flavor intel

# 启用颜色和美化输出
set style enabled on
set print pretty on
set print array-indexes on

# 自定义命令
define kreg
    echo \033[1;32m=== 关键寄存器 ===\033[0m\n
    info registers rax rbx rcx rdx rsi rdi rsp rbp rip
    echo \033[1;32m=== 段寄存器 ===\033[0m\n
    info registers cs ds es fs gs ss
end

define kmem
    if $argc == 0
        echo \033[1;32m=== 栈内存 (RSP附近) ===\033[0m\n
        x/20xw $rsp
    else
        if $argc == 1
            echo \033[1;32m=== 内存查看 ===\033[0m\n
            x/$arg0xw $rsp
        else
            echo \033[1;32m=== 指定地址内存 $arg1 ===\033[0m\n  
            x/$arg0xw $arg1
        end
    end
end

# 监视变量命令
define kvar
    if $argc == 1
        echo \033[1;32m=== 监视变量: $arg0 ===\033[0m\n
        print $arg0
        watch $arg0
    else
        echo "用法: kvar <变量名>\n"
    end
end

# 显示监视
define kwatchlist
    echo \033[1;32m=== 当前监视点 ===\033[0m\n
    info watchpoints
end

# 重启
define restart
    disconnect
    file out/Silan.kernel
    target remote localhost:1234
    break kernel_main
    echo "GDB已重启，QEMU保持运行\n"
end