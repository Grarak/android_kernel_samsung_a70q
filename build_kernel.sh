#!/bin/bash

set -e

export CROSS_COMPILE=~/android/laos/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android-

if [ ! -d out ]; then
	mkdir out
fi

BUILD_CROSS_COMPILE=~/android/laos/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android-
KERNEL_LLVM_BIN=~/android/laos/prebuilts/clang/host/linux-x86/clang-4691093/bin/clang
CLANG_TRIPLE=aarch64-linux-gnu-

#export ARCH=arm64
#make -C $(pwd) O=$(pwd)/out KCFLAGS=-mno-android sm6150_sec_defconfig
#make -j64 -C $(pwd) O=$(pwd)/out KCFLAGS=-mno-android

make -C $(pwd) O=$(pwd)/out ARCH=arm64 CROSS_COMPILE=$BUILD_CROSS_COMPILE REAL_CC=$KERNEL_LLVM_BIN CLANG_TRIPLE=$CLANG_TRIPLE a70q_defconfig
make -j16 -C $(pwd) O=$(pwd)/out ARCH=arm64 CROSS_COMPILE=$BUILD_CROSS_COMPILE REAL_CC=$KERNEL_LLVM_BIN CLANG_TRIPLE=$CLANG_TRIPLE

tools/mkdtimg create out/dtbo.img --page_size=4096 $(find out -name "*.dtbo")
