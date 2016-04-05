#include "threads.h"
#include "stdio.h"

struct thread_t threads[COUNT];

struct list_head free;
struct list_head busy;
struct list_head dead;

static struct thread_t* volatile cur = NULL;
static struct thread_t* volatile general = NULL;

void init() {
	start_atomic();

	list_init(&busy);
	list_init(&free);
	list_init(&dead);
	
	for (size_t i = 0; i < COUNT; ++i) {
		list_init(&threads[i].node);
		list_add_tail(&threads[i].node, &free);
	}

	cur = thread(NULL, NULL);
	cur->stat = RUNNING;

	end_atomic();
}

void set_general(struct thread_t* thread) { 
	cur = thread;
}

struct thread_t* thread(void* (*func)(void *), void *arg) {	
	start_atomic();

	struct thread_t* thread = LIST_ENTRY(list_first(&free), struct thread_t, node);
	list_del(list_first(&free));
 	
 	thread->stack_start = (uint8_t *)kmem_alloc(STACK_SIZE);
 	thread->stack = (void*)((uint8_t *)thread->stack_start + STACK_SIZE - 1);
	thread->func = func;
	thread->arg = arg;
	thread->result = NULL;
	thread->stat = NOT_STARTED;

	end_atomic();
	return thread;
}

struct thread_t* get_current() {
	return cur;
}

void terminate() {
	start_atomic();

	cur->stat = DIED;
	list_add_tail(&cur->node, &dead);
	
	end_atomic();
	yield();
}

void start(struct thread_t* thread) {
	start_atomic();

	list_add_tail(&thread->node, &busy);

	end_atomic();
}

void kill(struct thread_t* thread) {
	start_atomic();

	kmem_free(thread->stack_start);
	list_del(&thread->node);
	list_add_tail(&thread->node, &free);

	end_atomic();
}

void join(struct thread_t* thread, void** result) {
	while (thread->stat != DIED) {
		yield();
		barrier();
	}
	*result = thread->result;
}

void switch_context(struct thread_t * new_thread, struct thread_t *volatile* old_thread);

void yield() {
	start_atomic();

	if (cur != NULL && cur->stat == RUNNING) {
		list_add_tail(&cur->node, &busy);
	}

	struct thread_t* thread = LIST_ENTRY(list_first(&busy), struct thread_t, node);
	list_del(&thread->node);

	switch_context(thread, &cur);

	end_atomic();
}

void try_wrapper_entry(struct thread_t* thread) {
	if (thread->stat == NOT_STARTED) {
		thread->stat = RUNNING;
		end_atomic();
		thread->result = thread->func(thread->arg);
		terminate();
	}
}