#include "uart.h"
#include "pit.h"
#include "pic.h"
#include "idt.h"
#include "interrupt.h"
#include <stddef.h>

void printl(uint64_t a) {
	uint64_t b = 0;
	uint32_t n = 0;
	while (a > 0) {
		b *= 10;
		b += a % 10;
		a /= 10;
		n++;
	}
	if (b == 0) {
		putc('0');
	}
	while (b > 0) {
		putc(b % 10 + '0');
		b /= 10;
		n--;
	}
	for (uint32_t i = 0; i < n; ++i) {
		putc('0');
	}
}

void print(const char* s) {
	while (*s != 0) {
		putc(*s++);
	}
}

struct elem {
	uint64_t first;
	uint64_t size;
	uint32_t type;
};

uint64_t last(struct elem e) {
	return e.first + e.size;
}


extern uint32_t mboot_info;
extern char text_phys_begin[];
extern char bss_phys_end[];

uint32_t mmap_size = 0;
struct elem mmap[100];

void set_mmap(){
	
	struct elem kernel;
	kernel.first = (uint64_t) text_phys_begin;
	kernel.size = (uint64_t) bss_phys_end - (uint64_t) text_phys_begin;
	kernel.type = 0;
	uint32_t mmap_length = *(uint32_t *) (uint64_t) (mboot_info + 44);
	uint32_t mmap_first = *(uint32_t *) (uint64_t) (mboot_info + 48);
	uint32_t cur = 0;
	
	while (cur < mmap_length) {
		struct elem el = *(struct elem*)(uint64_t)(mmap_first + cur + 4);
		cur += *(uint32_t *)(uint64_t) (mmap_first + cur);
        cur += 4;
		if (el.first < kernel.first && last(el) > last(kernel)) {
			mmap[mmap_size] = el;
			mmap[mmap_size].size = kernel.first - el.first;
			++mmap_size;
			mmap[mmap_size] = kernel;
			++mmap_size;
			mmap[mmap_size] = el;
			mmap[mmap_size].first = last(kernel);
			mmap[mmap_size].size = last(el) - last(kernel);
		}
		else if (el.first < kernel.first && last(el) >= kernel.first) {
			el.size = kernel.first - el.first;
			mmap[mmap_size] = el;
			++mmap_size;
			mmap[mmap_size] = kernel;
		}
		else if (el.first <= last(kernel) && last(el) > last(kernel)) {
			el.size -= last(kernel) - el.first;
			el.first = last(kernel);
			mmap[mmap_size] = el;
		}
		else if (el.first >= kernel.first && last(el) <= last(kernel)) {
			continue;
		}
		else {
			mmap[mmap_size] = el;
		}
		++mmap_size;
	}
}

void print_mmap() {
	for (uint32_t i = 0; i < mmap_size; ++i) {
		printl(mmap[i].first);
		print (" ");
		printl(last(mmap[i]));
		print (" ");
		print(mmap[i].type == 1 ? "Available" : mmap[i].type == 0 ? "Kernel" : "Reserved");
		print ("\n");
	}
}

void repeat_me() {
	int n = 10000007;
	int f = 0;
	for (int i = 2; i < n; ++i) {
		if (n % i == 0) {
			f = i;
		}
	}
	if (f) {
		print("not prime\n");
	}
	else {
		print("prime\n");
	}
	//eoi();
}

void main(void) {
	init_uart();
	init_pic();
	init_idt();
	init_pit();
	sti();
	//set_idt_func(0x20, (uint64_t)&run_repeat_me);
	set_mmap();
	print_mmap();

	while (1); 
}
