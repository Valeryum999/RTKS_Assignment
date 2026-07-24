/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

/*
 * Shared benchmark interface, identical for wasm and native builds.
 * bench_init/bench_run: the kernel (results/<bench>/main.c).
 * bench_now_cycles/bench_report: provided by the WAMR host (imports) or by
 * native_provider.c (native build).
 */

#ifndef BENCH_H
#define BENCH_H

#include <stdint.h>

void     bench_sleep(uint32_t seconds);
void     bench_init(void);
void     bench_run(void);

uint32_t bench_now_cycles(void);
void     bench_report(uint32_t iter, uint32_t cycles);

void     bench_main(void);

#endif /* BENCH_H */
