# init/rules.mk

KERNEL_BIN_TARGET = $(BUILD_DIR)/kernel.bin

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
