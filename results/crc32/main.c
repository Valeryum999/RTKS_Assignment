/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <stdint.h>

#define BUFFER_SIZE (32 * 1024)

static uint8_t buffer[BUFFER_SIZE];

volatile uint32_t benchmark_result = 0;

void bench_init(void)
{
    uint32_t x = 1;

    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        x = x * 1664525u + 1013904223u;
        buffer[i] = (uint8_t)(x >> 24);
    }
}

static uint32_t crc32(const uint8_t *data, uint32_t length)
{
    uint32_t crc = 0xFFFFFFFF;

    for (uint32_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (int bit = 0; bit < 8; bit++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
    }
    return ~crc;
}

uint32_t bench_run(void)
{
    uint32_t crc = crc32(buffer, BUFFER_SIZE);
    benchmark_result = crc;
    return crc;
}
