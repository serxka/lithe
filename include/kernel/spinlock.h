#ifndef _KERNEL_SPINLOCK_H_
#define _KERNEL_SPINLOCK_H_

#include <kernel/common.h>

// TODO: fix me up for perf and C11 - PAUSE, C11 Atomics

typedef volatile struct spinlock_s {
	volatile u32 lock[1];
} spinlock_t;

#define spinlock_init(l)         \
	do {                     \
		(l).lock[0] = 0; \
	} while (0)

#define spinlock_take(l) while (__sync_lock_test_and_set((l).lock, 0x01))
#define spinlock_give(l) __sync_lock_release((l).lock)

#endif // _KERNEL_SPINLOCK_H_
