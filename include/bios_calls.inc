; bios_calls.inc BIOS 常用功能号定义

; 键盘输入功能号
KEY_INPUT_WAIT    equ 0x00    ; 等待键盘输入
KEY_INPUT_CHECK   equ 0x01    ; 检查键盘输入
KEY_INPUT_GET     equ 0x00    ; 获取键盘输入

; 显示功能号  
VIDEO_TELETYPE    equ 0x0E    ; 电传打字输出
VIDEO_SET_MODE    equ 0x00    ; 设置显示模式
VIDEO_GET_MODE    equ 0x0F    ; 获取显示模式

; 磁盘功能号
DISK_RESET        equ 0x00    ; 磁盘复位
DISK_READ         equ 0x02    ; 读磁盘扇区
DISK_WRITE        equ 0x03    ; 写磁盘扇区
DISK_GET_STATUS   equ 0x01    ; 获取磁盘状态

; 内存功能号
MEM_GET_SIZE      equ 0x88    ; 获取扩展内存大小

; 串口功能号
SERIAL_INIT       equ 0x00    ; 初始化串口
SERIAL_SEND       equ 0x01    ; 发送字符
SERIAL_RECV       equ 0x02    ; 接收字符