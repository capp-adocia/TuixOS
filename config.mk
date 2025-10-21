# config.mk

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
CFLAGS += -Wall -Wextra -Wpedantic

# 链接选项
LDFLAGS = -m elf_i386 -nostdlib -T linker.ld

# 自动查找源文件
C_SRCS = $(shell find $(SRC_DIRS) -name "*.c")
C_OBJS = $(C_SRCS:%.c=$(BUILD_DIR)/%.o)