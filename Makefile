# Makefile

all: disk.img

boot.bin: boot/boot.asm
	nasm -I include/ -f bin $< -o $@

disk.img: boot.bin
	dd if=/dev/zero of=disk.img bs=512 count=20480 2>/dev/null  # 创建10MB硬盘镜像
	dd if=boot.bin of=disk.img conv=notrunc 2>/dev/null
# 软盘启动
run: disk.img
	qemu-system-i386 -drive file=disk.img,format=raw,if=floppy

clean:
	rm -f boot.bin disk.img

.PHONY: all run clean