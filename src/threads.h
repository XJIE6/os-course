#ifndef __THREAD_H__
#define __THREAD_H__

#include "memory.h"
#include "kmem_cache.h"
#include "list.h"
#include "lock.h"

#define COUNT (1 << 10)
#define STACK_SIZE (1 << 10)

typedef enum {
	NOT_STARTED,
	RUNNING,
	DIED
} status;

struct thread_t {
	void* stack;
	void* (*func)(void*);
	void* result;
	void* arg;
	void* stack_start;
	status stat;
	struct list_head node;
};

void init();
void set_general(struct thread_t* thread);
struct thread_t* thread(void* (*func)(void *), void *arg);
void start(struct thread_t* thread);
void kill(struct thread_t* thread);
void join(struct thread_t* thread, void** result);
void yield();
struct thread_t* thread_get_current();

#endif