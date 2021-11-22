#pragma once

#include <lithe/base/defs.h>

typedef volatile struct spinlock_s { volatile uint32_t lock[1]; } spinlock_t;

#define spinlock_init(l)         \
	do {                     \
		(l).lock[0] = 0; \
	} while (0)

#define spinlock_take(l) while (__sync_lock_test_and_set((l).lock, 0x01))
#define spinlock_give(l) __sync_lock_release((l).lock)
