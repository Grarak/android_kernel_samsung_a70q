#!/bin/bash

set -e

export CROSS_COMPILE=~/android/laos16/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android-

mkdir out

BUILD_CROSS_COMPILE=~/android/laos16/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android-
KERNEL_LLVM_BIN=~/android/laos16/prebuilts/clang/host/linux-x86/clang-4691093/bin/clang
CLANG_TRIPLE=aarch64-linux-gnu-
KERNEL_MAKE_ENV="DTC_EXT=$(pwd)/tools/dtc CONFIG_BUILD_ARM64_DT_OVERLAY=y"

#export ARCH=arm64
#make -C $(pwd) O=$(pwd)/out KCFLAGS=-mno-android sm6150_sec_defconfig
#make -j64 -C $(pwd) O=$(pwd)/out KCFLAGS=-mno-android

make -C $(pwd) O=$(pwd)/out $KERNEL_MAKE_ENV ARCH=arm64 CROSS_COMPILE=$BUILD_CROSS_COMPILE REAL_CC=$KERNEL_LLVM_BIN CLANG_TRIPLE=$CLANG_TRIPLE a70q_eur_open_defconfig
make -j16 -C $(pwd) O=$(pwd)/out $KERNEL_MAKE_ENV ARCH=arm64 CROSS_COMPILE=$BUILD_CROSS_COMPILE REAL_CC=$KERNEL_LLVM_BIN CLANG_TRIPLE=$CLANG_TRIPLE

tools/mkdtimg create out/dtbo.img --page_size=4096 $(find out -name "*.dtbo")
