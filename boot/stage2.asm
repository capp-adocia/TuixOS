; boot/stage2.asm 代码在0x7E00-0x85FF

[BITS 16]
%ifndef DEBUG
[ORG 0x7E00]     
%endif

%include "bios_calls.inc"

memory_info_address equ 0x5000
stack_top equ 0x7000
kernel_addr equ 0x9000

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
    mov ax, 0x0000
    mov es, ax
    mov di, memory_info_address 
    mov ebx, 0
    mov dword [es:di + 20], 1
    mov byte [memory_entry_count], 0  ; 初始化计数器

.memory_loop:
    mov eax, 0xE820
    mov edx, 0x534D4150
    mov ecx, 24
    int 0x15
    jc .error

    cmp eax, 0x534D4150
    jne .error

    ; 增加条目计数
    inc byte [memory_entry_count]
    
    ; 移动到下一个条目
    add di, 24

    test ebx, ebx
    jnz .memory_loop

    mov si, detect_memory_success
    call print_string
.done:
    ret


.error:
    ; 错误处理
    mov si, detect_memory_error
    call print_string
    ret

; 加载C内核 - 最小循环版本
load_kernel:
    ; 第一段：柱面0, 磁头0, 扇区6-18 (13个扇区)
    mov ax, 0x900
    mov es, ax
    xor bx, bx
    mov ah, 0x02
    mov al, 55 ; 注意56个是极限,超过就超过了0x10000即(64KB的边界)了
    mov ch, 0
    mov cl, 6
    mov dh, 0
    mov dl, 0
    int 0x13
    jc .error
    
    mov si, kernel_load_success
    call print_string
    ret

.error:
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
    ; jmp gdt[0x08] + protected_mode_entry(偏移量)
    ; 注意这里gdt[0x08]访问的是代码段里面的基址，通过获得基址再将代码偏移相加，就得到了真正的物理地址了
    jmp 0x08:protected_mode_entry
    ; 如果是实模式下应该是段:偏移->物理地址

[BITS 32]
protected_mode_entry:
    mov ax, 0x10 ; 数据段选择子
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, stack_top ; 栈顶
    mov ebp, esp

    jmp kernel_addr
    jmp $

; 打印十六进制
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

memory_entry_count db 0
stage2_msg db "[INFO]: Stage 2 Loader: Hello from sector 2!", 13, 10, 0
detect_memory_success db "[INFO]: Detect memory success!", 13, 10, 0
detect_memory_error db "[ERROR]: Detect memory error!", 13, 10, 0
kernel_load_success db "[INFO]: Kernel load from sector 6- success!", 13, 10, 0
kernel_load_error db "[ERROR]: Kernel load from sector 6- error!", 13, 10, 0 
a20_success_msg db "[INFO]: A20: Enabled", 13, 10, 0
a20_failed_msg db "[ERROR]: A20 line is disabled! Cannot enter protected mode.", 13, 10, 0
entering_pmode_msg db "[INFO]: Entering protected mode...", 13, 10, 0
lba_success_msg db "[INFO]: lba_success_msg...", 13, 10, 0


; 内存布局
; 0x00000000-0x0009FBFF:   639KB  可用
; 0x0009FC00-0x0009FFFF:   1KB    保留 (EBDA)
; 0x000A0000-0x000EFFFF:   320KB  保留 (视频内存)
; 0x000F0000-0x000FFFFF:   64KB   保留 (BIOS)
; 0x00100000-0x07FDFFFF:   127.875MB 可用
; 0x07FE0000-0x07FFFFFF:   128KB  保留
; 0xFFFC0000-0xFFFFFFFF:   256KB  保留 (BIOS ROM)

; 设置gdt表：注意现在的gdt表中的代码段gdt[0x08]和数据段gdt[0x10]内容上有重叠，还无法做到内存保护 
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
