/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <stdint.h>

#define ARRAY_SIZE 4096

static int32_t array[ARRAY_SIZE];

volatile int32_t benchmark_result = 0;

void bench_init(void)
{
    uint32_t x = 1;

    for (int i = 0; i < ARRAY_SIZE; i++) {
        x = x * 1664525u + 1013904223u;
        array[i] = (int32_t)x;
    }
}

static void quicksort(int32_t *a, int lo, int hi)
{
    if (lo >= hi)
        return;

    int32_t pivot = a[hi];
    int i = lo - 1;

    for (int j = lo; j < hi; j++) {
        if (a[j] <= pivot) {
            i++;
            int32_t t = a[i];
            a[i] = a[j];
            a[j] = t;
        }
    }
    i++;
    int32_t t = a[i];
    a[i] = a[hi];
    a[hi] = t;

    quicksort(a, lo, i - 1);
    quicksort(a, i + 1, hi);
}

uint32_t bench_run(void)
{
    quicksort(array, 0, ARRAY_SIZE - 1);

    /* Checksum prevents dead-code elimination of the sort. */
    int32_t checksum = 0;
    for (int i = 0; i < ARRAY_SIZE; i++)
        checksum += array[i] ^ i;

    benchmark_result = checksum;
    return (uint32_t)checksum;
}
