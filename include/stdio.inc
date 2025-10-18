; stdio.inc - 标准输入输出函数

; 打印字符串
; 输入: SI = 字符串地址
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

; 打印换行
print_newline:
    mov ah, 0x0E
    mov al, 13
    int 0x10
    mov al, 10
    int 0x10
    ret

; 打印十六进制字节
; 输入: AL = 要打印的字节
print_hex_byte:
    push ax
    push bx
    mov bl, al
    shr al, 4
    call .print_digit
    mov al, bl
    and al, 0x0F
    call .print_digit
    pop bx
    pop ax
    ret
.print_digit:
    cmp al, 10
    jb .is_digit
    add al, 'A' - 10
    jmp .print
.is_digit:
    add al, '0'
.print:
    mov ah, 0x0E
    int 0x10
    ret