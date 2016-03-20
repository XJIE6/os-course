#ifndef __IDT_H__
#define __IDT_H__

#include <stdint.h>
#include "interrupt.h"

struct idt {
	uint16_t 	offset_0_15;
	uint16_t 	segment_selector; 
	uint8_t	 	interrupt_stack_table;
	uint8_t 	flags;
	uint16_t	offset_16_31;
	uint32_t	offset_32_63;
	uint32_t 	reserved;
} __attribute__((packed));

void init_idt();

void set_idt_func(uint8_t, uint64_t);

void run_repeat_me();

void empty();

void err();

static inline void sti()
{ __asm__ volatile ("sti");}

#endif