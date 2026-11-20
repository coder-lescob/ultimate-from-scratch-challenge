#!/bin/bash

# error handling
set -euo pipefail

# verify git is available
if ! command -v git >/dev/null; then
    # if not install git
    echo "git is not installed, proceding for install"

    if command -v pacman >/dev/null; then
        sudo pacman -S --needed git
    elif command -v apt >/dev/null; then
        sudo apt install git
    elif command -v dnf >/dev/null; then
        sudo dnf install git
    else
        echo "your package manager is not supported. try to install the package `git` manually and rerunning this etup script"
        exit 1
    fi
fi

# verify if qemu is installed and if not install it.
if ! command -v qemu-system-riscv32 >/dev/null; then
    # install it
    echo "qemu is not installed, installing..."

    if command -v pacman >/edv/null; then
        sudo pacman -S --needed qemu-system-riscv qemu-img
    elif command -v apt >/dev/null; then
        sudo apt install qemu-system-riscv qemu-utils
    elif command -v dnf >/dev/null; then
        sudo dnf install qemu-system-riscv qemu-img
    else
        echo "your package manager is not supported. try to install the packages `qemu-system-riscv` and `qemu-img` manually and rerunning this setup script"
        exit 1
    fi
fi

TOOLCHAIN="toolchain"
TOOLCHAIN_INSTALL="$HOME/opt/riscv32/"

if find "$TOOLCHAIN_INSTALL/bin/riscv32-unknown-linux-gnu-gcc" >/dev/null; then
    echo "risc-v toolchain already installed, nothing to do"
else
    echo "risc-v toolchain not installed, proceding to installation, this may take a while"
    
    # create the toolchain dir
    mkdir -p "$TOOLCHAIN"

    # clone the repo
    cd "$TOOLCHAIN"
    git clone https://github.com/riscv-collab/riscv-gnu-toolchain.git
    cd "riscv-gnu-toolchain"

    # configure the build system and run it
    # only build riscv32-unknown-linux-gnu-gcc
    ./configure --prefix="$TOOLCHAIN_INSTALL" \
      --enable-linux \
      --with-arch=rv32ima --with-abi=ilp32 \
      --with-languages=c --disable-gdb
    rm -f stamps/build-gcc-linux-stage1
    rm -rf build-gcc-linux-stage1
    make -j"$(nproc)" stamps/build-gcc-linux-stage1
    cd ../..
fi

# export gcc
export PATH="$TOOLCHAIN_INSTALL/bin:$PATH"
CROSS_COMPILE_HEAD="riscv32-unknown-linux-gnu-"

# first install the kernel if we need to
KDIR="kernel"
if [[ -d "$KDIR" ]]; then
    echo "Kernel already installed nothing to do"
else
    echo "Kernel not installed proceding to installation, this may take a while"

    # verify if bc is installed otherwise install it 
    if ! command -v bc >/dev/null; then
        if command -v pacman >/dev/null; then
            sudo pacman -S --needed bc
        elif command -v apt >/dev/null; then
            sudo apt install bc
        elif command -v dnf >/dev/null; then
            sudo dnf install bc
        else
            echo "Could not resolve host package manager, please install manually the package `bc`"
            exit 1
        fi
    fi
    
    # the version of the kernel to download
    KVER="7.3.2"
    KERNEL="linux-${KVER}"
    KERNCONFIG=".kernconfig"
    mkdir -p "$KDIR"
    
    # download the kernel
    curl -fL "https://cdn.kernel.org/pub/linux/kernel/v7.x/linux-${KVER}.tar.xz" -o "${KDIR}/${KERNEL}.tar.xz"
    tar -xf "${KDIR}/${KERNEL}.tar.xz" -C "$KDIR"

    # copy the configuration
    cp "$KERNCONFIG" "${KDIR}/${KERNEL}/.config"
    
    # kernel cross compiling flags
    CROSS_FLAGS="ARCH=riscv CROSS_COMPILE=$CROSS_COMPILE_HEAD"

    # saves and compiles it
    make -C "${KDIR}/${KERNEL}" $CROSS_FLAGS olddefconfig
    make -C "${KDIR}/${KERNEL}" -j"$(nproc)" $CROSS_FLAGS

    # copy the kernel image
    cp "${KDIR}/${KERNEL}/arch/riscv/boot/Image" "$KDIR"
fi

DISK="disk/disk.img"

if [[ -f "$DISK" ]]; then
    echo "disk already in place, nothing to do"
else
    echo "no disk found making new one"
    
    # create new disk
    mkdir -p disk/
    qemu-img create -f raw "$DISK" 64M
fi
