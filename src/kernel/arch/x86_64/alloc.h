#pragma once

#include <utils/base/attributes.h>
#include <utils/base/defs.h>
#include <utils/base/error.h>
#include <utils/base/result.h>

typedef result_t(uintptr_t, error_t) alloc_result_t;

void alloc_init(uint64_t mem_available);
alloc_result_t alloc_pages(bool physical, size_t page_count);
void alloc_free_pages(bool physical, uintptr_t addr, size_t page_count);

static INLINE alloc_result_t alloc_page(void) {
	return alloc_pages(true, 0x1);
}

static INLINE void alloc_free_page(uintptr_t addr) {
	alloc_free_pages(true, addr, 0x1);
}
