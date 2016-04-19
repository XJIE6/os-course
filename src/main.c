#include "kmem_cache.h"
#include "interrupt.h"
#include "memory.h"
#include "serial.h"
#include "paging.h"
#include "stdio.h"
#include "misc.h"
#include "time.h"
#include "threads.h"
#include "file.h"
#include "initramfs.h"
#include <stddef.h>

void test() {
	struct file_t* file = open("/123/456/789.0");
	char* a = (char*)kmem_alloc(sizeof(char) * 11);
	a = "abacabadaba";
	write(file, 0, 11, a);
	set_offset(file, 0);
	char* b = (char*)kmem_alloc(sizeof(char) * 11);
	close(file);
	int size = read(file, 0, 11, b);
	for (int i = 0; i < size; ++i) {
		if (a[i] != b[i]) {
			printf("fail");
			return;
		}
	}
	printf("ok");
}

void main(void) {
	setup_serial();
	setup_misc();
	setup_ints();
	setup_memory();
	setup_initramfs();
	setup_buddy();
	setup_paging();
	setup_alloc();
	setup_time();

	thread_init();
	file_init();
	read_initramfs();
	test();

	while(true);
}
