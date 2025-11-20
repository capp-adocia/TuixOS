# Makefile

# 工具链配置
CC = gcc
LD = ld
NASM = nasm

# 目录配置
BUILD_DIR = out
ISO_DIR = isofiles
SRC_DIRS = init mm lib boot kernel drivers

# 编译选项
CFLAGS = -ffreestanding -nostdlib -nostartfiles -nodefaultlibs
CFLAGS += -m32 -std=gnu99 -O1 -g -fno-pie -nostdinc
CFLAGS += -I include/
CFLAGS += -Wall -Wextra -Wpedantic

# 链接选项
LDFLAGS = -m elf_i386 -nostdlib -T kernel.ld

# 自动查找源文件
C_SRCS = $(shell find $(SRC_DIRS) -name "*.c")

# 生成目标文件路径
C_OBJS = $(C_SRCS:%.c=$(BUILD_DIR)/%.o)
ASM_OBJS = $(BUILD_DIR)/boot/kernel_entry.o

# 目标
KERNEL_ELF = $(BUILD_DIR)/TuiPlus.kernel
ISO_IMAGE = TuiPlus-os.iso

.PHONY: all clean run help

all: $(ISO_IMAGE)

# 创建可启动ISO
# 注意grub这里会将整个目录打包进iso文件里
$(ISO_IMAGE): $(KERNEL_ELF)
	grub-mkrescue -o $(ISO_IMAGE) .

# 编译汇编入口文件
$(BUILD_DIR)/boot/kernel_entry.o: boot/kernel_entry.asm
	@mkdir -p $(dir $@)
	$(NASM) -f elf32 $< -o $@

# C文件构建
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# 链接内核
$(KERNEL_ELF): $(ASM_OBJS) $(C_OBJS)
	$(LD) $(LDFLAGS) -o $@ $(ASM_OBJS) $(C_OBJS)

# 运行和调试
run: $(ISO_IMAGE)
	@echo "[QEMU] 启动系统..."
	qemu-system-i386 -m 128M -cdrom $(ISO_IMAGE) -serial stdio

# 串口调试
debug: $(ISO_IMAGE)
	@echo "[QEMU] 启动串口调试..."
	@echo "[GDB] gdb -x debug/debug-grub.gdb"
	qemu-system-i386 -m 128M -cdrom $(ISO_IMAGE) -serial stdio -s -S -display none

# 清理
clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR) $(ISO_IMAGE)

help:
	@echo "构建目标:"
	@echo "  all     - 构建完整系统"
	@echo "  run     - 构建并运行"
	@echo "  debug   - 构建并调试"
	@echo "  clean   - 清理所有生成文件"