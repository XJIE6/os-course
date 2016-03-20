#include "uart.h"

void init_uart() {
	out8(0x3f8 + 3, 1 << 7);
	out8(0x3f8 + 0, 0x09);
	out8(0x3f8 + 1, 0x00);
	out8(0x3f8 + 3, 1 + (1 << 1) + (1 << 2));
	out8(0x3f8 + 1, 0);
}

void putc(char c) {
	out8(0x3f8 + 0, c);
}