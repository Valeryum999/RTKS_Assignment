/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <stdint.h>
#include <stdio.h>

static int fib(int n)
{
    if (n <= 1)
        return n;
    return fib(n - 1) + fib(n - 2);
}

int main(void)
{
    volatile int result = fib(30);

    // (void)result;
    printf("The result is: %d\n", result);
    return 0;
}
