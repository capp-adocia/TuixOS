; boot.asm BIOS加载MBR

[BITS 16]        ; 生成16位代码（实模式）
[ORG 0x7C00]     ; 被加载到内存地址 0x7C00 (这个是当前的基址)

%include "bios_calls.inc"
; 常量
End2B equ 0xAA55

start:
    ; 在实模式下，内存地址=段寄存器(ds)×16+偏移地址(si)
    ; 注意代码里计算的都是相对地址，最终的物理地址还要加上ORG地址
    xor ax, ax
    mov ax, cs
    mov ds, ax
    
    ; 显示启动消息
    mov si, Booting_msg
    call print_string
    
    ; 尝试读取MBR（第一个扇区的内容）
    mov ax, 0x07E0; Ax = Ah + Al
    mov es, ax    ; ES:BX 是磁盘缓冲区的标准
    xor bx, bx    ; 偏移量
    
    mov ah, 0x02 ; 读取扇区
    mov al, 1    ; 读取1个扇区
    mov ch, 0x00 ; 柱面0
    mov cl, 0x01 ; 扇区1
    mov dh, 0x00 ; 磁头0
    mov dl, 0x00 ; 软盘启动

    int 0x13
    jc disk_error

    mov si, success_msg
    call print_string

    jmp $ ; 死循环

disk_error:
    mov si, disk_error_msg
    call print_string

    ; 显示错误代码（AH中的值）
    mov al, ah
    call print_hex_byte ; 打印十六进制数

    mov ah, KEY_INPUT_WAIT
    int 0x16

    ret

; 打印字符串
print_string:
    mov ah, VIDEO_TELETYPE ; BIOS 功能号：在屏幕上显示字符
.repeat:
    lodsb           ; mov al, [ds:si] inc si
    cmp al, 0       ; 检查是否是字符串结尾（0）
    je .done        ; 如果是0，跳转到结束
    int 0x10        ; 调用 BIOS 显示服务显示 AL 中的字符
    jmp .repeat     ; 继续处理下一个字符
.done:
    ret

; 换行函数
print_newline:
    mov ah, VIDEO_TELETYPE
    mov al, 13        ; 回车CR
    int 0x10
    mov al, 10        ; 换行LF  
    int 0x10
    ret

; 打印十六进制字节
print_hex_byte:
    push ax
    push bx
    
    mov bl, al ; 备份al的值
    shr al, 4  ; 取出高4位
    call .print_digit
    mov al, bl ; 恢复原值
    and al, 0x0F ; 取出低4位
    call .print_digit
    call print_newline

    pop bx
    pop ax
    ret
.print_digit:
    cmp al, 10
    jb .digit
    add al, 'A' - 10
    jmp .print
.digit:
    add al, '0'
.print:
    mov ah, VIDEO_TELETYPE
    int 0x10
    ret


; 调试暂停
debug_pause:
    mov si, debug_msg
    call print_string
    mov ah, KEY_INPUT_WAIT
    int 0x16                ; 暂停，等待键盘输入
    mov si, end_debug_msg
    call print_string
    ret

success_msg db "Sucessful disk read", 13, 10, 0
disk_error_msg db "Error disk read", 13, 10, 0 
Booting_msg db 'Sucessful Booting...', 13, 10, 0
debug_msg db 'Start Debug, debug info...', 13, 10, 0
end_debug_msg db 'End Debug, debug end...', 13, 10, 0

times 510 - ($ - $$) db 0 ; (510 - 已使用字节量)
dw End2B ; 必须写入0xAA55这个是BIOS判断标志