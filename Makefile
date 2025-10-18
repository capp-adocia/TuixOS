# Makefile

all: disk.img

# 引导用的纯二进制（不带 DEBUG 标志）
stage1.bin: boot/stage1.asm
	nasm -I include/ -f bin $< -o $@

stage2.bin: boot/stage2.asm
	nasm -I include/ -f bin $< -o $@

# 调试用的 ELF 文件（带 DEBUG 标志）
stage1.debug.o: boot/stage1.asm
	nasm -I include/ -f elf32 -g -F dwarf -dDEBUG $< -o $@

stage2.debug.o: boot/stage2.asm
	nasm -I include/ -f elf32 -g -F dwarf -dDEBUG $< -o $@

disk.img: stage1.bin stage2.bin
	dd if=/dev/zero of=disk.img bs=512 count=2880 2>/dev/null
	dd if=stage1.bin of=disk.img conv=notrunc 2>/dev/null
	dd if=stage2.bin of=disk.img conv=notrunc seek=1 2>/dev/null

# 软盘启动
run: disk.img
	qemu-system-i386 -drive file=disk.img,format=raw,if=floppy

# 调试模式（无图形化，等待GDB连接）
debug: disk.img  stage1.debug.o stage2.debug.o
	@echo "Starting QEMU in debug mode..."
	@echo "Open another terminal and run: gdb -x debug.gdb"
	@echo "Press Ctrl+A then X to exit QEMU"
	qemu-system-i386 -drive file=disk.img,format=raw,if=floppy  -s -S -nographic
	
clean:
	rm -f *.bin *.o disk.img

.PHONY: all run debug clean