/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <stdint.h>

#define BUFFER_SIZE (32 * 1024)

static uint8_t buffer[BUFFER_SIZE];
static uint32_t crc_table[256];

volatile uint32_t benchmark_result = 0;

void bench_init(void)
{
    uint32_t x = 1;

    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        x = x * 1664525u + 1013904223u;
        buffer[i] = (uint8_t)(x >> 24);
    }

    for (uint32_t i = 0; i < 256; i++) {
        uint32_t c = i;
        for (int k = 0; k < 8; k++)
            c = (c & 1) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
        crc_table[i] = c;
    }
}

static uint32_t crc32(const uint8_t *data, uint32_t length)
{
    uint32_t crc = 0xFFFFFFFF;

    for (uint32_t i = 0; i < length; i++)
        crc = crc_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);

    return ~crc;
}

void bench_run(void)
{
    benchmark_result = crc32(buffer, BUFFER_SIZE);
}
