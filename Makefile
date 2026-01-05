# Makefile

# 工具链配置
CC = gcc-11
LD = ld
NASM = nasm

# 目录配置
BUILD_DIR = out
ISO_DIR = iso
SRC_DIRS = init lib kernel
ASM_DIRS = boot kernel/asm user

# 编译选项
CFLAGS = -ffreestanding -nostdlib -nostartfiles -nodefaultlibs
CFLAGS += -m32 -std=gnu11 -O2 -ggdb
CFLAGS += -fno-pic -fno-builtin -fno-strict-aliasing
CFLAGS += -fno-pie -nostdinc -fno-omit-frame-pointer -fno-stack-protector
CFLAGS += -I include/
CFLAGS += -Wall -Wextra -Werror
CFLAGS += -MD

# 链接选项
LDFLAGS = -m elf_i386 -nostdlib -T kernel.ld

# 自动查找源文件
C_SRCS = $(shell find $(SRC_DIRS) -name "*.c")
ASM_SRCS = $(shell find $(ASM_DIRS) -name "*.asm")

# 生成目标文件路径
ASM_OBJS = $(patsubst %.asm, $(BUILD_DIR)/%.o, $(ASM_SRCS))
C_OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SRCS))
OBJS = $(ASM_OBJS) $(C_OBJS)  # 汇编文件要在前，确保入口点正确

# 目标
KERNEL_ELF = $(BUILD_DIR)/Tuix.kernel
ISO_IMAGE = Tuix-os.iso
.PHONY: all clean run help

all: $(ISO_IMAGE)

# 1. 编译汇编入口文件
$(BUILD_DIR)/%.o: %.asm
	@mkdir -p $(dir $@)
	$(NASM) -f elf32 -F dwarf $< -o $@

# 2. 编译C文件
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# 3. 链接内核
$(KERNEL_ELF): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

# 4. 创建可启动ISO
# 注意grub这里会将整个目录打包进iso文件里
$(ISO_IMAGE): $(KERNEL_ELF)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_ELF) $(ISO_DIR)/boot/kernel.bin
	cp boot/grub/grub.cfg $(ISO_DIR)/boot/grub/
	grub-mkrescue -o $(ISO_IMAGE) $(ISO_DIR)/
	rm -rf $(ISO_DIR)

# 5. 运行
run: $(ISO_IMAGE)
	@echo "[QEMU] 启动系统..."
	qemu-system-i386 -m 256M -cdrom $(ISO_IMAGE) -serial stdio 2>&1 | tee ./log

# 6. 串口调试
debug: $(ISO_IMAGE)
	@echo "[QEMU] 启动串口调试..."
	@echo "[GDB] gdb -x debug/debug-grub.gdb"
	qemu-system-i386 -m 256M -cdrom $(ISO_IMAGE) -serial stdio -s -S -display none

# 7. 清理
clean:
	rm -rf $(BUILD_DIR) $(ISO_IMAGE)

help:
	@echo "构建目标:"
	@echo "  all     - 构建完整系统"
	@echo "  run     - 构建并运行"
	@echo "  debug   - 构建并调试"
	@echo "  clean   - 清理所有生成文件"
