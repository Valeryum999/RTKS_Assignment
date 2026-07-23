/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <stdint.h>

volatile int benchmark_result = 0;

static int fib(int n)
{
    if (n <= 1)
        return n;
    return fib(n - 1) + fib(n - 2);
}

void bench_init(void)
{
}

uint32_t bench_run(void)
{
    int result = fib(30);
    benchmark_result = result;
    return (uint32_t)result;
}
