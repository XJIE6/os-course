#include "lock.h"
#include "interrupt.h"
#include "threads.h"

volatile uint32_t deep = 0;

void lock(struct lock_t* lock) {
	if (lock == NULL) {
		return;
	}

	const uint16_t ticket = __sync_fetch_and_add(&lock->users, 1);
	while (lock->ticket != ticket) {
		barrier();
		yield();
	}

	__sync_synchronize();
}

void unlock(struct lock_t* lock) {
	if (lock == NULL) {
		return;
	}

	__sync_synchronize();
    __sync_add_and_fetch(&lock->ticket, 1);
}

void start_atomic() {
	local_irq_disable();
	__sync_fetch_and_add(&deep, 1);
}

void end_atomic() {
	if (__sync_fetch_and_add(&deep, -1) == 1) {
		local_irq_enable();
	}
}