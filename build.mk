# get project root and file system root
ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
ROOTFS := $(ROOT)/rootfs

# get the compiler
TOOLCHAIN := $(ROOT)/toolchain/
RISCVCC   := $(TOOLCHAIN)/riscv32-unknown-linux-gnu-gcc

# c and ld flags
CFLAGS  := -Wall -Wextra -Wpedantic -Os
LDFLAGS := -static

# build dir
BUILD := $(ROOT)/build

$(BUILD)/%.o: $(ROOT)/%.c
	@mkdir -p $(dir $@)
	@$(RISCVCC) -c $(CFLAGS) $< -o $@
	@echo RISCVCC $(shell basename $@)