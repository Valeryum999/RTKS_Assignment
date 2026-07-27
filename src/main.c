/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "bh_platform.h"
#include "bh_assert.h"
#include "bh_log.h"
#include "wasm_export.h"
#include "zephyr/kernel.h"
#include "zephyr/syscalls/kernel.h"
#include "test_wasm.h"

#define CONFIG_GLOBAL_HEAP_BUF_SIZE WASM_GLOBAL_HEAP_SIZE
#define CONFIG_APP_STACK_SIZE 8192
#define CONFIG_APP_HEAP_SIZE 8192

#define CONFIG_MAIN_THREAD_STACK_SIZE 8192

/* Set at build time via -DBENCH_NAME / -DBENCH_MODE / -DBENCH_PLATFORM. */
#ifndef BENCH_NAME
#define BENCH_NAME "unknown"
#endif
#ifndef BENCH_MODE
#define BENCH_MODE "unknown"
#endif
#ifndef BENCH_PLATFORM
#define BENCH_PLATFORM "unknown"
#endif

#ifndef BENCH_ITERATIONS
#define BENCH_ITERATIONS 20
#endif

static void
bench_report(uint32 iter, uint64 cycles)
{
    uint64_t value_ns = k_cyc_to_ns_floor64((uint64_t)cycles);
    printf("%s,%s,%s,%u,%llu,%llu\n", BENCH_NAME, BENCH_MODE, BENCH_PLATFORM,
           iter, cycles, (unsigned long long)value_ns);
}

#if WASM_ENABLE_GLOBAL_HEAP_POOL != 0
static char global_heap_buf[CONFIG_GLOBAL_HEAP_BUF_SIZE] = { 0 };
#endif

void
iwasm_main(void *arg1, void *arg2, void *arg3)
{
    uint8 *wasm_file_buf = NULL;
    uint32 wasm_file_size;
    wasm_module_t wasm_module = NULL;
    wasm_module_inst_t wasm_module_inst = NULL;
    wasm_exec_env_t exec_env = NULL;
    wasm_function_inst_t func_main = NULL;
    wasm_function_inst_t bench_init = NULL;
    uint32 call_argv[1] = { 0 };
    RuntimeInitArgs init_args;
    char error_buf[128];
#if WASM_ENABLE_LOG != 0
    int log_verbose_level = 2;
#endif

    (void)arg1;
    (void)arg2;
    (void)arg3;

    memset(&init_args, 0, sizeof(RuntimeInitArgs));

#if WASM_ENABLE_GLOBAL_HEAP_POOL != 0
    init_args.mem_alloc_type = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
    init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
#elif (defined(CONFIG_COMMON_LIBC_MALLOC)            \
       && CONFIG_COMMON_LIBC_MALLOC_ARENA_SIZE != 0) \
    || defined(CONFIG_NEWLIB_LIBC)
    init_args.mem_alloc_type = Alloc_With_System_Allocator;
#else
#error "memory allocation scheme is not defined."
#endif

    if (!wasm_runtime_full_init(&init_args)) {
        printf("Init runtime environment failed.\n");
        return;
    }

#if WASM_ENABLE_LOG != 0
    bh_log_set_verbose_level(log_verbose_level);
#endif

    wasm_file_buf = (uint8 *)wasm_test_file;
    wasm_file_size = sizeof(wasm_test_file);

    if (!(wasm_module = wasm_runtime_load(wasm_file_buf, wasm_file_size,
                                          error_buf, sizeof(error_buf)))) {
        printf("%s\n", error_buf);
        goto fail1;
    }

    if (!(wasm_module_inst = wasm_runtime_instantiate(
              wasm_module, CONFIG_APP_STACK_SIZE, CONFIG_APP_HEAP_SIZE,
              error_buf, sizeof(error_buf)))) {
        printf("%s\n", error_buf);
        goto fail2;
    }

    func_main = wasm_runtime_lookup_function(wasm_module_inst, "bench_run");
    if (!func_main) {
        printf("Failed to lookup exported function bench_run\n");
        goto fail2;
    }

    bench_init = wasm_runtime_lookup_function(wasm_module_inst, "bench_init");
    if (!func_main) {
        printf("Failed to lookup exported function bench_init\n");
        goto fail2;
    }

    exec_env =
        wasm_runtime_create_exec_env(wasm_module_inst, CONFIG_APP_STACK_SIZE);
    if (!exec_env) {
        printf("Create exec env failed\n");
        goto fail2;
    }

    printf("benchmark,mode,platform,iteration,value_cycles,value_ns\n");
    for(int i=0; i<BENCH_ITERATIONS; i++){
        if (!wasm_runtime_call_wasm(exec_env, bench_init, 0, call_argv)) {
            const char *exception = wasm_runtime_get_exception(wasm_module_inst);
            if (exception)
                printf("%s\n", exception);
        }
        uint32_t before = k_cycle_get_32();
        if (!wasm_runtime_call_wasm(exec_env, func_main, 0, call_argv)) {
            const char *exception = wasm_runtime_get_exception(wasm_module_inst);
            if (exception)
                printf("%s\n", exception);
        }
        uint32_t after = k_cycle_get_32();
        bench_report(i, after - before);
        k_sleep(K_SECONDS(1));
    }

    wasm_runtime_destroy_exec_env(exec_env);
    wasm_runtime_deinstantiate(wasm_module_inst);

fail2:
    wasm_runtime_unload(wasm_module);

fail1:
    wasm_runtime_destroy();
}

#define MAIN_THREAD_STACK_SIZE (CONFIG_MAIN_THREAD_STACK_SIZE)
#define MAIN_THREAD_PRIORITY 5

K_THREAD_STACK_DEFINE(iwasm_main_thread_stack, MAIN_THREAD_STACK_SIZE);
static struct k_thread iwasm_main_thread;

bool
iwasm_init(void)
{
    k_tid_t tid = k_thread_create(
        &iwasm_main_thread, iwasm_main_thread_stack, MAIN_THREAD_STACK_SIZE,
        iwasm_main, NULL, NULL, NULL, MAIN_THREAD_PRIORITY, 0, K_NO_WAIT);
    return tid ? true : false;
}

#if KERNEL_VERSION_NUMBER < 0x030400 /* version 3.4.0 */
void
main(void)
{
    iwasm_init();
}
#else
int
main(void)
{
    iwasm_init();
    return 0;
}
#endif
