/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <stdint.h>
#include "zephyr/kernel.h"

#ifndef BENCH_NAME
#define BENCH_NAME "unknown"
#endif
#ifndef BENCH_MODE
#define BENCH_MODE "native"
#endif
#ifndef BENCH_PLATFORM
#define BENCH_PLATFORM "unknown"
#endif

#ifndef BENCH_ITERATIONS
#define BENCH_ITERATIONS 20
#endif

void bench_report(uint32_t iter, uint64_t cycles)
{
    uint64_t value_ns = k_cyc_to_ns_floor64((uint64_t)cycles);
    printf("%s,%s,%s,%u,%llu,%llu\n", BENCH_NAME, BENCH_MODE, BENCH_PLATFORM,
           iter, cycles, (unsigned long long)value_ns);
}

void bench_init(void);
void bench_run(void);

int main(void)
{
    printf("benchmark,mode,platform,iteration,value_cycles,value_ns\n");
    for (uint32_t i = 0; i < BENCH_ITERATIONS; i++) {
        bench_init();

        uint64_t before = k_cycle_get_64();
        bench_run();
        uint64_t after = k_cycle_get_64();

        bench_report(i, after - before);
        k_sleep(K_SECONDS(1));
    }
    return 0;
}
