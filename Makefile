# Makefile

# 包含各个模块的配置
include config.mk
include boot/rules.mk
include init/rules.mk

all: $(BUILD_DIR)/disk.img

# 磁盘镜像
$(BUILD_DIR)/disk.img: $(BOOT_BIN_TARGETS) $(KERNEL_BIN_TARGET)
	@echo "[IMG]  制作磁盘镜像..."
	dd if=/dev/zero of=$@ bs=512 count=2880 2>/dev/null
	dd if=$(BUILD_DIR)/stage1.bin of=$@ conv=notrunc 2>/dev/null
	dd if=$(BUILD_DIR)/stage2.bin of=$@ conv=notrunc bs=512 seek=1 2>/dev/null
	dd if=$(BUILD_DIR)/kernel.bin of=$@ conv=notrunc bs=512 seek=5 2>/dev/null

# 运行和调试
run: $(BUILD_DIR)/disk.img
	@echo "[QEMU] 启动系统..."
	qemu-system-i386 -drive file=$<,format=raw,if=floppy

debug: $(BUILD_DIR)/disk.img $(BOOT_DEBUG_TARGETS) $(BUILD_DIR)/kernel.elf
	@echo "[DEBUG] 启动调试模式..."
	qemu-system-i386 -drive file=$(BUILD_DIR)/disk.img,format=raw,if=floppy -s -S -nographic

clean:
	rm -rf $(BUILD_DIR)/

help:
	@echo "构建目标:"
	@echo "  all     - 构建完整系统"
	@echo "  run     - 构建并运行"
	@echo "  debug   - 调试模式"
	@echo "  clean   - 清理"

.PHONY: all run debug clean help