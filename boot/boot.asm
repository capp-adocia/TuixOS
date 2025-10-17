; boot.asm BIOS加载MBR

[BITS 16]        ; 生成16位代码（实模式）
[ORG 0x7C00]     ; 被加载到内存地址 0x7C00
%include "bios_calls.inc"

    ; 在实模式下，内存地址=段寄存器(ds)×16+偏移地址(si)
    ; 注意代码里计算的都是相对地址，最终的物理地址还要加上ORG地址
    mov ax, cs
    mov ds, ax

    mov al, menu_msg ; 报错
    call print_string

    mov ah, KEY_INPUT_WAIT
    int 0x16 ; 等待键盘输入

    jmp $ ; 死循环

; 打印字符串
print_string:
    mov ah, 0x0E ; BIOS 功能号：在屏幕上显示字符
.repeat:
    lodsb           ; mov al, [ds:si] inc si
    cmp al, 0       ; 检查是否是字符串结尾（0）
    je .done        ; 如果是0，跳转到结束
    int 0x10        ; 调用 BIOS 显示服务显示 AL 中的字符
    jmp .repeat     ; 继续处理下一个字符
.done:
    ret

; 调试暂停
debug_pause:
    mov si, debug_msg
    call print_string
    mov ah, 0x00
    int 0x16
    ret

press_key_msg db 'Press any key to boot...', 13, 10, 0
hello_msg db 'Hello World! Booting...', 13, 10, 0
menu_msg db 'Start Menu, please choose which one...', 13, 10, 0
debug_msg db 'Start Debug, debug info...', 13, 10, 0

times 510 - ($ - $$) db 0 ; (510 - 已使用字节量)
dw 0xAA55 ; 必须写入0xAA55这个是BIOS判断标志