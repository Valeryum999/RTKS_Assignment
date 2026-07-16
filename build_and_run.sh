#!/bin/bash

# Copyright (C) 2019 Intel Corporation.  All rights reserved.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

X86_TARGET="x86"
ESP32C3_TARGET="esp32c3"

set -euxo;      

usage ()
{
        echo "USAGE:"
        echo "$0 |$ESP32C3_TARGET"
        echo "Example:"
        echo "        $0 $ESP32C3_TARGET"
        exit 1
}

if [ $# != 1 ] ; then
        usage
fi

TARGET=$1

case $TARGET in
        $ESP32C3_TARGET)
                west build -b esp32c3_devkitm \
                           . -p always -- \
                           -DWAMR_BUILD_TARGET=RISCV32_ILP32 \
                           -DWAMR_BUILD_AOT=0
                # west flash will discover the device
                west flash
                ;;
        *)
                src/wasm-src/build.sh
                west build -b rpi_pico2/rp2350a/m33 \
                            . -p always -- \
                            -DWAMR_BUILD_TARGET=THUMBV8 \
                            -DWAMR_BUILD_AOT=0
                west flash --openocd $HOME/openocd-bin/openocd
                ;;
esac

