-include build.mk

# get the kernel and define its arguments
KERNEL_DIR := kernel
KERNEL     := $(KERNEL_DIR)/Image
KERNELARGS := console=ttyS0 rdinit=/init

# the subdirectories
SUBDIRS := init sh

# the cpio archive and emulator
CPIO := $(BUILD)/rootfs.cpio
QEMU := qemu-system-riscv32

# disk image
DISK := $(BUILD)/disk.img

# compile all
build: $(SUBDIRS) cpio

cpio:
	@mkdir -p $(ROOTFS)/bin $(ROOTFS)/dev $(ROOTFS)/proc $(ROOTFS)/sys $(ROOTFS)/tmp $(ROOTFS)/mnt
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
		-drive file=$(DISK),format=raw,if=virtio \
		-append "$(KERNELARGS)"


$(SUBDIRS):
	$(MAKE) -C $@

clean:
	@rm -rf build
	@echo CLEAN

.PHONY: build $(SUBDIRS)