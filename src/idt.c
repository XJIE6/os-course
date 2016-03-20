#include "idt.h"

static const uint8_t err_nums[] = {8, 10, 11, 12, 13, 14, 17};

static struct idt idt[64];
static struct idt_ptr ptr;

void set_idt_func(uint8_t index, uint64_t offset) {
	idt[index].reserved = 0;
	idt[index].segment_selector = 0x18;
	idt[index].interrupt_stack_table = 0;
	idt[index].flags = 14 + (1 << 7);
	idt[index].offset_0_15 = offset;
	idt[index].offset_16_31	= offset >> 16;
	idt[index].offset_32_63	= offset >> 32;
}

void init_idt() {
	int j = 0;
	for (int i = 0; i < 64; i++) {
		if (j < 7 && err_nums[j] == i) {
			set_idt_func(i, (uint64_t)&err);
			j++;
		}
		else {
			set_idt_func(i, (uint64_t)&empty);
		}
	}

	ptr.base = (uint64_t)idt;
	ptr.size = sizeof(idt) - 1;
	set_idt(&ptr);
}
