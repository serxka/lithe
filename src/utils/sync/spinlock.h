#pragma once

#include <utils/base/defs.h>

typedef volatile uint32_t spinlock_t[1];

#define spinlock_init$(l) (l)[0] = 0x0

#define spinlock_take$(l)                                   \
	do {                                                \
		__asm__ __volatile__("pause" ::: "memory"); \
	} while (__sync_lock_test_and_set((l), 0x1))
#define spinlock_give$(l) __sync_lock_release((l))
