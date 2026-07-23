/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

/*Native baseline (BENCH_MODE=native) */

#include <stdint.h>
#include <zephyr/kernel.h>

#include "bench.h"

#ifndef BENCH_NAME
#define BENCH_NAME "unknown"
#endif
#ifndef BENCH_MODE
#define BENCH_MODE "native"
#endif
#ifndef BENCH_PLATFORM
#define BENCH_PLATFORM "unknown"
#endif

uint32_t bench_now_cycles(void)
{
    return k_cycle_get_32();
}

void bench_report(uint32_t iter, uint32_t cycles)
{
    uint64_t value_ns = k_cyc_to_ns_floor64((uint64_t)cycles);
    printf("%s,%s,%s,%u,%u,%llu\n", BENCH_NAME, BENCH_MODE, BENCH_PLATFORM,
           iter, cycles, (unsigned long long)value_ns);
}

int main(void)
{
    printf("benchmark,mode,platform,iteration,value_cycles,value_ns\n");
    bench_main();
    return 0;
}
