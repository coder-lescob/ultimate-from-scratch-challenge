-include build.mk

# get the kernel and define its arguments
KERNEL_DIR := kernel
KERNEL     := $(KERNEL_DIR)/Image
KERNELARGS := console=ttyS0 rdinit=/init

# the subdirectories
SUBDIRS := init

# the cpio archive and emulator
CPIO := $(BUILD)/rootfs.cpio
QEMU := qemu-system-riscv32

# compile all
build: $(SUBDIRS) cpio

cpio:
	@cd $(ROOTFS) && find . -print | cpio -o -H newc > $(CPIO) && cd ..
	@echo CPIO ARCHIVE CREATED

run: build
	@echo LAUNCHING QEMU
	@$(QEMU) \
		-M virt \
		-m 128M \
		-kernel $(KERNEL) \
		-initrd $(CPIO) \
		-display gtk \
		-serial stdio \
		-device bochs-display \
		-append "$(KERNELARGS)"


$(SUBDIRS):
	$(MAKE) -C $@

clean:
	rm -rf $(BUILD)

.PHONY: build $(SUBDIRS)