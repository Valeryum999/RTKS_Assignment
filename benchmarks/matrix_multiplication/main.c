/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <stdint.h>

#define MATRIX_SIZE 64

static float A[MATRIX_SIZE][MATRIX_SIZE];
static float B[MATRIX_SIZE][MATRIX_SIZE];
static float C[MATRIX_SIZE][MATRIX_SIZE];

volatile float benchmark_result = 0.0f;

void bench_init(void)
{
    float seed = 1.234567f;

    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            seed = seed * 1.3141593f + 0.2718282f;
            while (seed >= 100.0f)
                seed -= 100.0f;
            A[i][j] = seed;

            seed = seed * 1.3141593f + 0.2718282f;
            while (seed >= 100.0f)
                seed -= 100.0f;
            B[i][j] = seed;
            C[i][j] = 0.0f;
        }
    }
}

void bench_run(void)
{
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            float sum = 0.0f;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }

    float checksum = 0.0f;

    for (int i = 0; i < MATRIX_SIZE; i++)
        for (int j = 0; j < MATRIX_SIZE; j++)
            checksum += C[i][j];

    benchmark_result = checksum;
}
