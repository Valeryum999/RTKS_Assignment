#!/bin/bash

# Copyright (C) 2019 Intel Corporation.  All rights reserved.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set -euo pipefail

usage ()
{
        cat <<'EOF'
USAGE:
  build_and_run.sh [--mode MODE] [--target TARGET] [--bench BENCH]

  --mode    interp | fast_interp | aot | native   (default: interp)
            native = same kernel/loop compiled straight to the CPU, no WAMR
            (the baseline). Requires --bench.
  --target  rpi_pico2_thumb | rpi_pico2_riscv32 | esp32c6_riscv32
            (default: rpi_pico2_thumb)
  --bench   fibonacci | crc32 | matrix_multiplication | quicksort
            For the wasm modes: when given, benchmarks/<bench>/main.c is
            copied into src/wasm-src/main.c before building; otherwise the
            current src/wasm-src/main.c is used and the CSV name is "wasm".
            Required for --mode native.

Examples:
  build_and_run.sh --mode aot --target rpi_pico2_thumb --bench crc32
  build_and_run.sh --mode fast_interp --target rpi_pico2_riscv32 --bench quicksort
  build_and_run.sh --mode native --target rpi_pico2_thumb --bench quicksort
EOF
        exit 1
}

MODE="interp"
TARGET="rpi_pico2_thumb"
BENCH=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        --mode)   MODE="$2";   shift 2 ;;
        --target) TARGET="$2"; shift 2 ;;
        --bench)  BENCH="$2";  shift 2 ;;
        -h|--help) usage ;;
        *) echo "Unknown argument: $1" >&2; usage ;;
    esac
done

# --- Map the target to board name, WAMR target and AOT (wamrc) target --------
case "$TARGET" in
    rpi_pico2_thumb)
        WAMR_BUILD_TARGET="THUMBV8"
        BOARD="rpi_pico2/rp2350a/m33"
        WAMRC_TARGET_ARGS="--target=thumbv8m.main"
        PLATFORM="rpi_pico2_thumb"
        ;;
    rpi_pico2_riscv32)
        WAMR_BUILD_TARGET="RISCV32_ILP32"
        BOARD="rpi_pico2/rp2350a/hazard3"
        WAMRC_TARGET_ARGS="--target=riscv32 --target-abi=ilp32 --cpu=generic-rv32 --cpu-features=+i,+m,+a,+c,-f,-d"
        PLATFORM="rpi_pico2_riscv32"
        ;;
    esp32c6_riscv32)
        WAMR_BUILD_TARGET="RISCV32_ILP32"
        BOARD="esp32c6_devkitc/esp32c6/hpcore"
        WAMRC_TARGET_ARGS="--target=riscv32 --target-abi=ilp32 --cpu=generic-rv32 --cpu-features=+i,+m,+a,+c,-f,-d"
        PLATFORM="esp32c6_riscv32"
        ;;
    *) echo "Unknown target: $TARGET" >&2; usage ;;
esac

# --- Map the mode to WAMR build flags ---------------------------------------
IS_AOT=0
IS_NATIVE=0
case "$MODE" in
    interp)
        WAMR_FLAGS="-DWAMR_BUILD_INTERP=1 -DWAMR_BUILD_AOT=0 -DWAMR_BUILD_FAST_INTERP=0"
        ;;
    fast_interp)
        WAMR_FLAGS="-DWAMR_BUILD_INTERP=1 -DWAMR_BUILD_AOT=0 -DWAMR_BUILD_FAST_INTERP=1"
        ;;
    aot)
        WAMR_FLAGS="-DWAMR_BUILD_INTERP=0 -DWAMR_BUILD_AOT=1"
        IS_AOT=1
        ;;
    native)
        IS_NATIVE=1
        ;;
    *) echo "Unknown mode: $MODE" >&2; usage ;;
esac

# --- Native baseline: no wasm pipeline, no WAMR -----------------------------
if [[ "$IS_NATIVE" == "1" ]]; then
    if [[ -z "$BENCH" ]]; then
        echo "--mode native requires --bench" >&2
        exit 1
    fi
    BENCH_SRC="benchmarks/${BENCH}/main.c"
    if [[ ! -f "$BENCH_SRC" ]]; then
        echo "No such benchmark source: $BENCH_SRC" >&2
        exit 1
    fi
    echo "==> mode=native target=$TARGET bench=$BENCH board=$BOARD"
    west build -b "$BOARD" \
               . -p always -- \
               -DBENCH_MODE=native \
               -DBENCH_SRC="$BENCH_SRC" \
               -DBENCH_NAME="$BENCH" \
               -DBENCH_PLATFORM="$PLATFORM"
    west flash --openocd "$HOME/openocd-bin/openocd"
    exit 0
fi

# --- Select the benchmark source --------------------------------------------
BENCH_NAME="wasm"
if [[ -n "$BENCH" ]]; then
    SRC="benchmarks/${BENCH}/main.c"
    if [[ ! -f "$SRC" ]]; then
        echo "No such benchmark source: $SRC" >&2
        exit 1
    fi
    cp -a "$SRC" src/wasm-src/main.c
    BENCH_NAME="$BENCH"
fi

echo "==> mode=$MODE target=$TARGET bench=$BENCH_NAME board=$BOARD"

# --- Build the wasm module and generate the C header ------------------------
src/wasm-src/build.sh
if [[ "$IS_AOT" == "1" ]]; then
    docker run --rm -v "$PWD":/data wamrc ${WAMRC_TARGET_ARGS} \
        -o /data/src/wasm-src/out.aot /data/src/wasm-src/test.wasm
    src/wasm-src/build/binarydump -o "${PWD}/src/test_wasm.h" \
        -n wasm_test_file "${PWD}/src/wasm-src/out.aot"
else
    src/wasm-src/build/binarydump -o "${PWD}/src/test_wasm.h" \
        -n wasm_test_file "${PWD}/src/wasm-src/test.wasm"
fi

# --- Build for the board and flash ------------------------------------------
west build -b "$BOARD" \
           . -p always -- \
           -DWAMR_BUILD_TARGET="$WAMR_BUILD_TARGET" \
           ${WAMR_FLAGS} \
           -DBENCH_NAME="$BENCH_NAME" \
           -DBENCH_MODE="$MODE" \
           -DBENCH_PLATFORM="$PLATFORM"
west flash --openocd "$HOME/openocd-bin/openocd"
