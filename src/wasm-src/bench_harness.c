/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "bench.h"

#ifndef BENCH_ITERATIONS
#define BENCH_ITERATIONS 20
#endif

void bench_main(void)
{
    for (uint32_t i = 0; i < BENCH_ITERATIONS; i++) {
        bench_init();

        uint32_t before = bench_now_cycles();
        bench_run();
        uint32_t after = bench_now_cycles();

        bench_report(i, after - before);
    }
}
