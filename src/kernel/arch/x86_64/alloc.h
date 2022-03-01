#pragma once

#include <lithe/base/defs.h>
#include <lithe/base/error.h>
#include <lithe/base/result.h>

typedef result(uintptr_t, error) alloc_result;

void alloc_init(uint64_t mem_available);
void alloc_frame_set(uint64_t addr);
void alloc_frame_clear(uint64_t addr);
bool alloc_frame_test(uint64_t addr);
alloc_result alloc_first_frame(void);
