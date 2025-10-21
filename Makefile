# Makefile

# 工具链配置
CC = gcc
LD = ld
NASM = nasm
OBJCOPY = objcopy

# 目录配置
BUILD_DIR = out
SRC_DIRS = init mm lib boot

# 编译选项
CFLAGS = -ffreestanding -nostdlib -nostartfiles -nodefaultlibs
CFLAGS += -m32 -std=gnu99 -O0 -g -fno-pie -nostdinc
CFLAGS += -I include/
CFLAGS += -Wall -Wextra -Wpedantic -Wshadow -Wpointer-arith 
CFLAGS += -Wmissing-prototypes -Wunreachable-code
CFLAGS += -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion

# 链接选项
LDFLAGS = -m elf_i386 -nostdlib -T linker.ld

# 自动查找源文件
C_SRCS = $(shell find $(SRC_DIRS) -name "*.c")
ASM_SRCS = $(shell find boot -name "*.asm")

# 生成目标文件路径
C_OBJS = $(C_SRCS:%.c=$(BUILD_DIR)/%.o)
BIN_TARGETS = $(BUILD_DIR)/stage1.bin $(BUILD_DIR)/stage2.bin $(BUILD_DIR)/kernel.bin
DEBUG_ELF_TARGETS = $(BUILD_DIR)/stage1d.elf $(BUILD_DIR)/stage2d.elf

all: $(BUILD_DIR)/disk.img

# 引导文件构建
$(BUILD_DIR)/%.bin: boot/%.asm
	@echo "[ASM]  $< -> $@"
	@mkdir -p $(dir $@)
	$(NASM) -I include/ -f bin $< -o $@

# C文件构建
$(BUILD_DIR)/%.o: %.c
	@echo "[CC]   $< -> $@"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# 内核ELF和BIN
$(BUILD_DIR)/kernel.elf: $(C_OBJS) linker.ld
	@echo "[LD]   链接内核 -> $@"
	$(LD) $(LDFLAGS) -o $@ $(C_OBJS)

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel.elf
	@echo "[BIN]  $< -> $@"
	$(OBJCOPY) -O binary $< $@

# 调试ELF文件
$(BUILD_DIR)/stage1d.elf: boot/stage1.asm
	@echo "[ASM-DBG] $< -> $@"
	@mkdir -p $(dir $@)
	$(NASM) -I include/ -f elf32 -g -F dwarf -dDEBUG $< -o $@

$(BUILD_DIR)/stage2d.elf: boot/stage2.asm
	@echo "[ASM-DBG] $< -> $@"
	@mkdir -p $(dir $@)
	$(NASM) -I include/ -f elf32 -g -F dwarf -dDEBUG $< -o $@

# 磁盘镜像
$(BUILD_DIR)/disk.img: $(BIN_TARGETS)
	@echo "[IMG]  制作磁盘镜像..."
	dd if=/dev/zero of=$@ bs=512 count=2880 2>/dev/null
	dd if=$(BUILD_DIR)/stage1.bin of=$@ conv=notrunc 2>/dev/null
	dd if=$(BUILD_DIR)/stage2.bin of=$@ conv=notrunc bs=512 seek=1 2>/dev/null
	dd if=$(BUILD_DIR)/kernel.bin of=$@ conv=notrunc bs=512 seek=5 2>/dev/null

# 运行和调试
run: $(BUILD_DIR)/disk.img
	@echo "[QEMU] 启动系统..."
	qemu-system-i386 -drive file=$<,format=raw,if=floppy

debug: $(BUILD_DIR)/disk.img $(DEBUG_ELF_TARGETS) $(BUILD_DIR)/kernel.elf
	@echo "[DEBUG] 启动调试模式..."
	@echo "在另一个终端运行: gdb -x debug/debug.gdb"
	qemu-system-i386 -drive file=$(BUILD_DIR)/disk.img,format=raw,if=floppy -s -S -nographic

# 清理
clean:
	rm -rf $(BUILD_DIR)/

help:
	@echo "构建目标:"
	@echo "  all     - 构建完整系统"
	@echo "  run     - 构建并运行"
	@echo "  debug   - 构建调试版本并启动QEMU+GDB"
	@echo "  clean   - 清理所有生成文件"

.PHONY: all run debug clean help