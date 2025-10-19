# Makefile

# 工具链配置
CC = gcc
LD = ld
NASM = nasm
OBJCOPY = objcopy

# 编译选项
CFLAGS = -ffreestanding -nostdlib -nostartfiles -nodefaultlibs
CFLAGS += -m32 -std=gnu99 -O1 -g -I include/
CFLAGS += -fno-pie

CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -Wshadow -Wpointer-arith 
CFLAGS += -Wmissing-prototypes
CFLAGS += -Wunreachable-code

# 内核开发特殊豁免
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-unused-function
CFLAGS += -Wno-sign-conversion

# CFLAGS += -Werror

# 链接选项
LDFLAGS = -m elf_i386 -nostdlib -T linker.ld

# 目标文件
OBJS = kernel.o

all: disk.img

# 引导MBR用的纯二进制（不带 DEBUG 标志）
stage1.bin: boot/stage1.asm
	@echo "[ASM]  $<"
	$(NASM) -I include/ -f bin $< -o $@

# 第二阶段加载器（汇编）
stage2.bin: boot/stage2.asm
	@echo "[ASM]  $<"
	$(NASM) -I include/ -f bin $< -o $@

# 编译C内核
kernel.o: init/main.c
	@echo "[CC]  $<"
	$(CC) $(CFLAGS) -c $< -o $@

# 编译C内核为ELF，然后转为纯二进制
kernel.bin: $(OBJS) linker.ld
	@echo "[LD]  $(OBJS) -> kernel.elf"
	$(LD) $(LDFLAGS) -o kernel.elf $(OBJS)
	$(OBJCOPY) -O binary kernel.elf kernel.bin

# 调试用的 ELF 文件（带 DEBUG 标志）
stage1d.o: boot/stage1.asm
	$(NASM) -I include/ -f elf32 -g -F dwarf -dDEBUG $< -o $@

stage2d.o: boot/stage2.asm
	$(NASM) -I include/ -f elf32 -g -F dwarf -dDEBUG $< -o $@

kernel.elf: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o kernel.elf $(OBJS)

disk.img: stage1.bin stage2.bin kernel.bin
	@echo "[DD]  $<"
	dd if=/dev/zero of=disk.img bs=512 count=2880 2>/dev/null
	dd if=stage1.bin of=disk.img conv=notrunc 2>/dev/null
	dd if=stage2.bin of=disk.img conv=notrunc bs=512 seek=1 2>/dev/null
	dd if=kernel.bin of=disk.img conv=notrunc bs=512 seek=5 2>/dev/null

# 软盘启动
run: disk.img
	@echo "[RUNNING...]  $<"
	qemu-system-i386 -drive file=disk.img,format=raw,if=floppy

# 调试模式（无图形化，等待GDB连接）
debug: disk.img stage1d.o stage2d.o kernel.elf
	@echo "Starting QEMU in debug mode..."
	@echo "Open another terminal and run: gdb -x debug.gdb"
	@echo "Press Ctrl+A then X to exit QEMU"
	qemu-system-i386 -drive file=disk.img,format=raw,if=floppy  -s -S -nographic
	
clean:
	rm -f *.bin *.o *.log *.img *.elf


help:
	@echo "Available Object:"
	@echo "  all     - 构建完整系统"
	@echo "  run     - 构建并运行"
	@echo "  debug   - 构建调试版本并启动QEMU+GDB"
	@echo "  clean   - 清理所有生成文件"

.PHONY: all run debug clean help