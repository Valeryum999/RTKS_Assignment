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

static void init_matrices(void)
{
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            A[i][j] = (float)((i + j) % 17);
            B[i][j] = (float)((i * 3 + j * 5) % 23);
            C[i][j] = 0.0f;
        }
    }
}

int main(void)
{
    init_matrices();
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
    return 0;
}