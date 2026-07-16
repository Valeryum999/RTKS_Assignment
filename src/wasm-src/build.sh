# Copyright (C) 2019 Intel Corporation.  All rights reserved.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
set -euxo;

WAMR_DIR=${PWD}/../wasm-micro-runtime

echo "Build wasm app .."
/home/valeryum/Downloads/wasi-sdk/wasi-sdk-33.0-x86_64-linux/bin/clang -O3 \
        -z stack-size=128 -Wl,--initial-memory=65536 \
        -Wl,--global-base=0 \
        -o ${PWD}/src/wasm-src/test.wasm ${PWD}/src/wasm-src/main.c \
        -Wl,--export=main -Wl,--export=__main_argc_argv \
        -Wl,--export=__data_end -Wl,--export=__heap_base \
        -Wl,--strip-all,--no-entry \
        -Wl,--allow-undefined \
        -nostdlib \

# echo "Build binarydump tool .."
# rm -fr build && mkdir build && cd build
# cmake ${WAMR_DIR}/test-tools/binarydump-tool
# make
# cd ..

echo "Generate test_wasm.h .."
${PWD}/src/wasm-src/build/binarydump -o ${PWD}/src/wasm-src/test_wasm.h -n wasm_test_file ${PWD}/src/wasm-src/test.wasm
cp -a ${PWD}/src/wasm-src/test_wasm.h ${PWD}/src/test_wasm.h

echo "Done"
