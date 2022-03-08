#pragma once

#include <utils/base/defs.h>
#include <utils/base/error.h>
#include <utils/base/result.h>

typedef result_t(uintptr_t, error_t) alloc_result_t;

void alloc_init(uint64_t mem_available);
void alloc_frame_set(uint64_t addr);
void alloc_frame_clear(uint64_t addr);
bool alloc_frame_test(uint64_t addr);
alloc_result_t alloc_first_frame(void);
