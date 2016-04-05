#include "kmem_cache.h"
#include "interrupt.h"
#include "memory.h"
#include "serial.h"
#include "paging.h"
#include "stdio.h"
#include "misc.h"
#include "time.h"
#include "threads.h"
#include <stddef.h>

uint32_t b = 1;
uint32_t c = 0;
struct lock_t* l;

void* foo(void * x) {
	int param = *((int *) x);
	while (c == 0) {};
	lock(l);
	for (int i = 0; i < param; ++i) {
		b += 1;
		barrier();
	}
	c = 2;
	unlock(l);
	while (c == 2) {};
	lock(l);
	for (int i = 0; i < param; ++i) {
		b += 1;
		barrier();
	}
	c = 4;
	unlock(l);
	return x;
} 

void* bar(void * y) {
	int param = *((int *) y);
	lock(l);
	b *= param;
	c = 1;
	unlock(l);
	while (c == 1) {};
	lock(l);
	b *= 1 + param;
	c = 3;
	unlock(l);
	while (c == 3) {};
	lock(l);
	b *= 2 + param;
	unlock(l);
	return y;
} 

void test() {
	int a = 123456;
	int aa = 2;
	struct thread_t* t1 = thread(&foo, &a);
	struct thread_t* t2 = thread(&bar, &aa);
	start(t1);
	start(t2);
	void * res1, *res2;
	join(t1, &res1);
	join(t2, &res2);
	kill(t1);
	kill(t2);
	printf("%d == 1975320", b);
}

void main(void) {
	setup_serial();
	setup_misc();
	setup_ints();
	setup_memory();
	setup_buddy();
	setup_paging();
	setup_alloc();
	setup_time();

	init();
	test();
	while(true);
}
