#!/usr/bin/env bash

set -e

modes=(
    native
    aot
    fast_interp
    interp
)

benches=(
    crc32
    quicksort
    matrix_multiplication
    fibonacci
)

for mode in "${modes[@]}"; do
    for bench in "${benches[@]}"; do
        echo "========================================"
        echo "Running mode=$mode bench=$bench"
        echo "========================================"

        ./build_and_run.sh --mode "$mode" --target thumbv8 --bench "$bench"

        echo "Waiting 30 seconds..."
        sleep 30
    done
done

echo "All benchmarks completed."