# Makefile

include mkconf/utils.mk

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
CFLAGS += -m32 -std=gnu11 -O2 -g
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

ENTRY_ASM = boot/kernel_entry.asm
OTHER_ASM_SRCS = $(filter-out $(ENTRY_ASM), $(ASM_SRCS))

# 生成目标文件路径
ASM_OBJS = $(BUILD_DIR)/$(ENTRY_ASM:.asm=.o) \
           $(patsubst %.asm, $(BUILD_DIR)/%.o, $(OTHER_ASM_SRCS))
C_OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SRCS))
OBJS = $(ASM_OBJS) $(C_OBJS)  # 汇编文件要在前

# 目标
KERNEL_ELF = $(BUILD_DIR)/Tuix.kernel
ISO_IMAGE = Tuix-os.iso
DISK_IMAGE = fs.img
.PHONY: all clean run help

all: $(ISO_IMAGE) $(DISK_IMAGE)

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
	$(call log_info, 创建ISO文件...)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_ELF) $(ISO_DIR)/boot/kernel.bin
	cp boot/grub/grub.cfg $(ISO_DIR)/boot/grub/
	grub-mkrescue -o $(ISO_IMAGE) $(ISO_DIR)/
	rm -rf $(ISO_DIR)
	$(call log_ok, ISO文件创建完成: $(ISO_IMAGE))

# 5. 创建
$(DISK_IMAGE):
	$(call log_info, 创建硬盘镜像...)
	dd if=/dev/zero of=$(DISK_IMAGE) bs=1M count=32
	# TODO:调用mkfs工具进行格式化
	# ./mkfs $(DISK_IMAGE)
	$(call log_ok, 磁盘镜像创建完成: $(DISK_IMAGE))

# 6. 运行
run: $(ISO_IMAGE) $(DISK_IMAGE)
	$(call log_info, 启动QEMU...)
	qemu-system-i386 -m 256M \
		-cdrom $(ISO_IMAGE) \
		-drive file=$(DISK_IMAGE),format=raw \
		-serial stdio 2>&1 | tee ./log

# 7. 串口调试
debug: $(ISO_IMAGE)
	$(call log_info, 启动串口调试...)
	$(call log_info, gdb -x debug/debug-grub.gdb)
	qemu-system-i386 -m 256M \
		-cdrom $(ISO_IMAGE) \
		-drive file=$(DISK_IMAGE),format=raw \
		-serial stdio -s -S -display none
# 8. 清理
clean:
	$(call log_info, 清理以下文件...)
	rm -rf $(BUILD_DIR) $(ISO_IMAGE) $(DISK_IMAGE)
	$(call log_ok, 清理完成)

help:
	$(call log_info, 构建目标:)
	$(call log_info, all   - 构建完整系统)
	$(call log_info, run   - 构建并运行)
	$(call log_info, debug - 构建并调试)
	$(call log_info, clean - 清理所有生成文件)
