#!/bin/bash

# Copyright (C) 2019 Intel Corporation.  All rights reserved.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception      

usage ()
{
        echo "USAGE:"
        echo "$0 | aot"
        echo "Examples:"
        echo "  $0"
        echo "  $0 aot"
        exit 1
}

IS_AOT=$1

src/wasm-src/build.sh
if [[ "$IS_AOT" == "aot" ]]; then
    docker run --rm -v "$PWD":/data wamrc --target=thumbv8m.main -o /data/src/wasm-src/out.aot /data/src/wasm-src/test.wasm
    src/wasm-src/build/binarydump -o ${PWD}/src/test_wasm.h -n wasm_test_file ${PWD}/src/wasm-src/out.aot
else
    src/wasm-src/build/binarydump -o ${PWD}/src/test_wasm.h -n wasm_test_file ${PWD}/src/wasm-src/test.wasm
fi

west build -b rpi_pico2/rp2350a/m33 \
           . -p always -- \
           -DWAMR_BUILD_TARGET=THUMBV8
west flash --openocd $HOME/openocd-bin/openocd


