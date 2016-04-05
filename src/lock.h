#ifndef __LOCK_H__
#define __LOCK_H__

#include <stdint.h>

static inline void barrier() {
    __asm__ volatile ("" : : : "memory");
}

struct lock_t {
	uint16_t users;
	uint16_t ticket;
};

void lock(struct lock_t* lock);
void unlock(struct lock_t* lock);
void start_atomic();
void end_atomic();

#endif