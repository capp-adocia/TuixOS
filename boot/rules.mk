# boot/rules.mk

BOOT_BIN_TARGETS = $(BUILD_DIR)/stage1.bin $(BUILD_DIR)/stage2.bin
BOOT_DEBUG_TARGETS = $(BUILD_DIR)/stage1d.elf $(BUILD_DIR)/stage2d.elf

# 引导文件构建
$(BUILD_DIR)/%.bin: boot/%.asm
	@echo "[ASM]  $< -> $@"
	@mkdir -p $(dir $@)
	$(NASM) -I include/ -f bin $< -o $@

# 调试ELF文件
$(BUILD_DIR)/stage1d.elf: boot/stage1.asm
	@echo "[ASM-DBG] $< -> $@"
	@mkdir -p $(dir $@)
	$(NASM) -I include/ -f elf32 -g -F dwarf -dDEBUG $< -o $@

$(BUILD_DIR)/stage2d.elf: boot/stage2.asm
	@echo "[ASM-DBG] $< -> $@"
	@mkdir -p $(dir $@)
	$(NASM) -I include/ -f elf32 -g -F dwarf -dDEBUG $< -o $@
