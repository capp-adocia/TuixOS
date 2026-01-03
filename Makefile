# Makefile

# 工具链配置
CC = gcc-11
LD = ld
NASM = nasm

# 目录配置
BUILD_DIR = out
ISO_DIR = isofiles
SRC_DIRS = init mm lib kernel drivers
ASM_DIRS = boot kernel/asm

# 编译选项
CFLAGS = -ffreestanding -nostdlib -nostartfiles -nodefaultlibs
CFLAGS += -m32 -std=gnu11 -O1 -ggdb3 -fno-pie -nostdinc -fno-omit-frame-pointer -fno-stack-protector 
CFLAGS += -I include/
CFLAGS += -Wall -Wextra -Wpedantic

# 链接选项
LDFLAGS = -m elf_i386 -nostdlib -T kernel.ld

# 自动查找源文件
C_SRCS = $(shell find $(SRC_DIRS) -name "*.c")
ASM_SRCS = $(shell find $(ASM_DIRS) -name "*.asm")

# 生成目标文件路径
C_OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SRCS))
ASM_OBJS = $(patsubst %.asm, $(BUILD_DIR)/%.o, $(ASM_SRCS))

OBJS = $(ASM_OBJS) $(C_OBJS)  # 汇编文件要在前，确保入口点正确

# 目标
KERNEL_ELF = $(BUILD_DIR)/Tuix.kernel
ISO_IMAGE = Tuix-os.iso

.PHONY: all clean run help

all: $(ISO_IMAGE)

# 创建可启动ISO
# 注意grub这里会将整个目录打包进iso文件里
$(ISO_IMAGE): $(KERNEL_ELF)
	grub-mkrescue -o $(ISO_IMAGE) .

# 编译汇编入口文件
$(BUILD_DIR)/%.o: %.asm
	@mkdir -p $(dir $@)
	$(NASM) -f elf32 $< -o $@

# C文件构建
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# 链接内核
$(KERNEL_ELF): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

# 运行和调试
run: $(ISO_IMAGE)
	@echo "[QEMU] 启动系统..."
	qemu-system-i386 -m 128M -cdrom $(ISO_IMAGE) -serial stdio 2>&1 | tee ./log

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
