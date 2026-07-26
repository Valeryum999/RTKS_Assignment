# Copyright (C) 2019 Intel Corporation.  All rights reserved.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
set -euxo pipefail

# Override WASI_SDK_CLANG if the WASI SDK lives elsewhere.
WASI_SDK_CLANG="${WASI_SDK_CLANG:-/home/valeryum/Downloads/wasi-sdk/wasi-sdk-33.0-x86_64-linux/bin/clang}"

echo "Build wasm app .."
"${WASI_SDK_CLANG}" -O3 \
        -I"${PWD}/src/wasm-src" \
        -z stack-size=8192 -Wl,--initial-memory=65536 \
        -Wl,--global-base=0 \
        -o "${PWD}/src/wasm-src/test.wasm" \
        "${PWD}/src/wasm-src/main.c" \
        -Wl,--export=bench_run -Wl,--export=bench_init \
        -Wl,--export=__data_end -Wl,--export=__heap_base \
        -Wl,--strip-all,--no-entry \
        -Wl,--allow-undefined \
        -nostdlib

# echo "Build binarydump tool .."
# rm -fr build && mkdir build && cd build
# cmake ${WAMR_DIR}/test-tools/binarydump-tool
# make
# cd ..

# echo "Generate test_wasm.h .."
# ${PWD}/src/wasm-src/build/binarydump -o ${PWD}/src/wasm-src/test_wasm.h -n wasm_test_file ${PWD}/src/wasm-src/test.wasm
# cp -a ${PWD}/src/wasm-src/test_wasm.h ${PWD}/src/test_wasm.h

# echo "Done"
