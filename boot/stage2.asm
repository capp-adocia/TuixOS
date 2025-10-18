; stage2.asm

[BITS 16]
%ifndef DEBUG
[ORG 0x7E00]     
%endif

%include "bios_calls.inc"

start:
    mov ax, cs
    mov ds, ax

    mov si, stage2_msg
    call print_string

    jmp $ ; 死循环

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

stage2_msg db "Stage 2 Loader: Hello from sector 2!", 13, 10, 0

; 填充到4个扇区（2048字节）
times 2048 - ($ - $$) db 0