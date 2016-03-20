#include "pit.h"

void init_pit() {
	out8(0x43, (1 << 2) + (1 << 4) + (1 << 5));
	out8(0x40, 0xFF);
	out8(0x40, 0xFF);
}