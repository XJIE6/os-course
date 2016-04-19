#ifndef __FILE_H__
#define __FILE_H__

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

struct file_t {
	char* name;
	struct file_t* next;
	struct file_t* child;
	char* data;
	uint32_t capasity;
	uint32_t offset;
	uint32_t size;
	struct lock_t* file_lock;
	bool dir;
};

void file_init();
void set_offset(struct file_t* file, uint32_t offset);
struct file_t* open(char* name);
void close(struct file_t* file);
uint32_t read(struct file_t* file, uint32_t offset, uint32_t size, char* buffer);
void write(struct file_t* file, uint32_t offset, uint32_t size, char* buffer);
void mkdir(char* name);
struct file_t readdir(char* name);

#endif