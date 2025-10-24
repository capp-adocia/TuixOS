; boot/stage2.asm 代码在0x7E00-0x85FF

[BITS 16]
%ifndef DEBUG
[ORG 0x7E00]     
%endif

%include "bios_calls.inc"

memory_info_address equ 0x5000

start:
    mov ax, cs
    mov ds, ax

    mov si, stage2_msg
    call print_string

    ; 检测内存布局
    call detect_memory
    ; 创建 GDT 数据结构
    ; 准备进入保护模式
    call prepare_pmode

    jmp $ ; 死循环

; 内存检测
; struct e820_entry {
;    uint64_t base_addr;   // 8字节 - 内存起始地址
;    uint64_t length;      // 8字节 - 内存区域长度  
;    uint32_t type;        // 4字节 - 内存类型
;    uint32_t acpi_attrs;  // 4字节 - ACPI 属性
; };
detect_memory:
    ; ES:DI 目标缓冲区地址，DI用于结构体、数组指针
    ; 使用 INT 0x15, AX=0xE820
    mov ax, 0x0000
    mov es, ax
    mov di, memory_info_address ; 将内存信息保存到0x5000
    mov ebx, 0
    mov dword [es:di + 20], 1 ; 设置有效的ACPI条目

.memory_loop:
    mov eax, 0xE820
    mov edx, 0x534D4150
    mov ecx, 24
    int 0x15
    jc .error

    cmp eax, 0x534D4150
    jne .error

    ; 移动到下一个条目
    add di, 24

    ; 检查是否完成，eax=0表示完成
    test ebx, ebx
    jnz .memory_loop

    ; 成功完成
    mov si, detect_memory_success
    call print_string
.done:
    ; mov ax, di
    ; sub ax, 0x5000
    ; mov bl, 24
    ; div bl
    ; mov [0x4F00], al
    ; 这里计算出条目有6个 即6*24B
    ret

.error:
    ; 错误处理
    mov si, detect_memory_error
    call print_string
    ret

; 加载C内核
load_kernel:
    mov ax, 0x900      ; 内核加载到 0x9000（注意这里不是写0x9000）
    mov es, ax
    xor bx, bx
    
    mov ah, 0x02        ; 读取扇区
    mov al, 10          ; 读取10个扇区（5KB内核）
    mov ch, 0           ; 柱面0
    mov cl, 6           ; 从扇区6开始（stage2在2-5）
    mov dh, 0           ; 磁头0
    mov dl, 0           ; 驱动器0
    int 0x13
    jc .error
    mov si, kernel_load_success
    call print_string
    ret
.error:
    ; 错误处理
    mov si, kernel_load_error
    call print_string
    jmp $

; 开启 A20
enable_a20:
    call test_a20_status
    cmp ax, 1 ; ax=1表示A20已开启
    je .success

    mov si, a20_failed_msg
    call print_string
    jmp $
.success:
    mov si, a20_success_msg
    call print_string
    ret

; 检测A20的状态
test_a20_status:
    ; 返回: AX=1 (开启) 或 AX=0 (关闭)
    push es
    push fs
    push di
    push si
    
    mov ax, 0x0000
    mov es, ax
    mov di, 0x0500      ; ES:DI = 0x0000:0x0500
    
    mov ax, 0xFFFF
    mov fs, ax
    mov si, 0x0510      ; FS:SI = 0xFFFF:0x0510 = 0x100000
    
    ; 保存原始值
    mov al, [es:di]
    push ax
    mov al, [fs:si]
    push ax
    
    ; 写入测试值
    mov byte [es:di], 0x00
    mov byte [fs:si], 0xFF
    
    ; 添加延迟确保写入完成
    mov cx, 0x100
.delay:
    nop
    loop .delay
    
    ; 检查是否相同
    mov al, [es:di]
    cmp al, 0xFF        ; 如果相同，A20 关闭
    je .a20_off
    
.a20_on:
    mov ax, 1           ; 返回 1 表示开启
    jmp .restore

.a20_off:
    mov ax, 0           ; 返回 0 表示关闭

.restore:
    ; 恢复原始值
    pop bx
    mov [fs:si], bl
    pop bx
    mov [es:di], bl
    
    pop si
    pop di
    pop fs
    pop es
    ret

; 准备进入保护模式
prepare_pmode:
    ; 加载C内核
    call load_kernel
    ; 启用A20
    call enable_a20

    mov si, entering_pmode_msg
    call print_string
    
    lgdt [gdt_descriptor]
    
    cli ; 关中断

    mov eax, cr0
    or eax, 1
    mov cr0, eax
    ; 选择子:偏移 注意此时已经是保护模式了
    jmp 0x08:protected_mode_entry
    ; 如果是实模式下应该是段:偏移->物理地址

[BITS 32]
protected_mode_entry:
    mov ax, 0x10 ; 数据段选择子
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x90000 ; 栈顶
    mov ebp, esp

    ; call pm_clear_screen
    ; call pm_print_string
    ; call pm_set_cursor
    ; 进入c代码
    jmp 0x9000
    jmp $

; 保护模式下的清屏函数
pm_clear_screen:
    mov edi, 0xB8000     ; 显存字符的首地址
    mov ecx, 80 * 25     ; 80x25文本模式
    mov eax, 0x0F200F20  ; 黑底白字的空格
.clear_loop:
    mov [edi], eax
    add edi, 4 ; 每次处理2字符，一个字符2字节
    loop .clear_loop
    ret

; 保护模式下的字符串打印
pm_print_string:
    mov esi, pm_message ; 这里esi指向字符串的地址，不是存储它的内容
    mov edi, 0xB8000
    mov ah, 0x0F  ; 黑字白底
.print_loop:
    lodsb
    test al, al
    jz .done
    mov [edi], ax ; ax = 属性字节 + 字符字节
    add edi, 2    ; 目标地址自增
    jmp .print_loop
.done:
    ret

; 设置光标位置 (行=row, 列=col)
; 文本模式光标位置 = row * 80 + col
pm_set_cursor:
    push eax
    push edx
    push ebx
    ; 计算光标位置
    mov eax, [current_row]
    mov ebx, 80
    mul ebx
    add eax, [current_col]

    ; 设置光标位置
    mov ebx, eax
    ; 向VGA寄存器写入光标位置低字节
    mov dx, 0x3D4
    mov al, 0x0F
    out dx, al
    mov dx, 0x3D5
    mov al, bl
    out dx, al

    ; 向VGA寄存器写入光标位置高字节
    mov dx, 0x3D4
    mov al, 0x0E
    out dx, al
    mov dx, 0x3D5
    mov al, bl
    out dx, al

    pop ebx
    pop edx
    pop eax
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

stage2_msg db "[INFO]: Stage 2 Loader: Hello from sector 2!", 13, 10, 0
detect_memory_success db "[INFO]: Detect memory success!", 13, 10, 0
detect_memory_error db "[ERROR]: Detect memory error!", 13, 10, 0
kernel_load_success db "[INFO]: Kernel load from sector 6-15 success!", 13, 10, 0
kernel_load_error db "[ERROR]: Kernel load from sector 6-15 error!", 13, 10, 0 
a20_success_msg db "[INFO]: A20: Enabled", 13, 10, 0
a20_failed_msg db "[ERROR]: A20 line is disabled! Cannot enter protected mode.", 13, 10, 0
entering_pmode_msg db "[INFO]: Entering protected mode...", 13, 10, 0
pm_message db "[INFO]: Stage 3 Loader, Protected Mode init success!", 0
current_row dd 0
current_col dd 0


; 内存布局
; 0x00000000-0x0009FBFF:   639KB  可用
; 0x0009FC00-0x0009FFFF:   1KB    保留 (EBDA)
; 0x000A0000-0x000EFFFF:   320KB  保留 (视频内存)
; 0x000F0000-0x000FFFFF:   64KB   保留 (BIOS)
; 0x00100000-0x07FDFFFF:   127.875MB 可用
; 0x07FE0000-0x07FFFFFF:   128KB  保留
; 0xFFFC0000-0xFFFFFFFF:   256KB  保留 (BIOS ROM)

gdt_start:
    dq 0x0000000000000000    ; 空描述符
    
    ; 代码段 (0x08): 4GB, 执行/读
    dw 0xFFFF                ; Limit 0:15
    dw 0x0000                ; Base 0:15
    db 0x00                  ; Base 16:23  
    db 0x9A                  ; P=1, DPL=0, Code, Execute/Read
    db 0xCF                  ; G=1, D=1, Limit 16:19=0xF
    db 0x00                  ; Base 24:31
    
    ; 数据段 (0x10): 4GB, 读/写  
    dw 0xFFFF                ; Limit 0:15
    dw 0x0000                ; Base 0:15
    db 0x00                  ; Base 16:23
    db 0x92                  ; P=1, DPL=0, Data, Read/Write
    db 0xCF                  ; G=1, D=1, Limit 16:19=0xF
    db 0x00                  ; Base 24:31
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; GDT 界限
    dd gdt_start                ; GDT 基地址
; 填充到4个扇区（2048字节）
times 2048 - ($ - $$) db 0