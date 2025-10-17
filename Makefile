# Makefile

all: boot.bin

boot.bin: boot/boot.asm
	nasm -I include/ -f bin $< -o $@

run: boot.bin
	qemu-system-i386 -drive format=raw,file=$<

clean:
	rm -f *.bin

.PHONY: all run clean