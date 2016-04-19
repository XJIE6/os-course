#include "file.h"
#include "interrupt.h"
#include "memory.h"
#include "serial.h"
#include "paging.h"
#include "stdio.h"
#include "lock.h"

struct file_t* head;

struct file_t* init() {
	struct file_t* file = (struct file_t *)kmem_alloc(sizeof(struct file_t));
	file->name = (char *)kmem_alloc(1<<10);
	file->next = NULL;
	file->child = NULL;
	file->data = (char *)kmem_alloc(1<<10);
	file->capasity = 1<<10;
	file->size = 0;
	file->offset = 0;
	file->file_lock = (struct lock_t*)kmem_alloc(sizeof(struct lock_t));
	file->dir = true;
	return file;
}

char* parse(char* c) {
	if (*c == 0) {
		return c;
	}
	while (*c != 0 && *c != '/') {
		c++;
	}
	return c;
}

bool cmp(char* s, char* fst, char* snd) {
	while (*s != 0 && fst != snd) {
		if (*s != *fst) {
			return false;
		}
		s++;
		fst++;
	}
	return (*s == 0 && fst == snd);
}

void file_init() {
	head = init();
}

struct file_t* mkfile(struct file_t* file, char* name) {
	char* end = parse(name + 1);
	struct file_t* node = init();
	node->next = file->child;
	file->child = node;
	uint32_t cur = 0;
	while (name != end) {
		node->name[cur] = *name;
		name++;
		cur++;
	}
	if (*end == 0) {
		node->dir = false;
		return node;
	}
	return mkfile(node, name);
}

struct file_t* open(char* name){
	char* fst = name, *snd;
	snd = parse(fst + 1);
	struct file_t* cur = head->child, *prev = head;
	while (fst != snd) {
		if (cur == NULL) {
			break;
		}
		if (cmp(cur->name, fst, snd)) {
			prev = cur;
			cur = cur->child;
			char* k = parse(snd + 1);
			fst = snd;
			snd = k;
		}
		else {
			cur = cur->next;
		}
	}
	if (cur == NULL) {
		cur = mkfile(prev, fst);
	}
	lock(cur->file_lock);
	return cur;
}
void close(struct file_t* file) {
	unlock(file->file_lock);
}
void set_offset(struct file_t* file, uint32_t offset) {
	file->offset = offset;
}
uint32_t read(struct file_t* file, uint32_t offset, uint32_t size, char* buffer){
	uint32_t cur = 0;
	while (file->offset < file->size && cur < size) {
		buffer[file->offset] = file->data[offset + cur];
		file->offset++;
		cur++;
	}
	return cur;
}
void realloc(struct file_t* file) {
	char* data = (char *)kmem_alloc(file->capasity * 2);
	for (uint32_t i = 0; i < file->capasity; ++i) {
		data[i] = file->data[i];
	}
	kmem_free(file->data);
	file->data = data;
	file->capasity *= 2;
}
void write(struct file_t* file, uint32_t offset, uint32_t size, char* buffer){
	while (file->capasity - file->offset < size - offset) {
		realloc(file);
	}
	uint32_t cur = 0;
	while (cur < size) {
		file->data[file->offset] = buffer[cur + offset];
		cur++;
		file->offset++;
	}
	file->size = file->offset;
}
void mkdir(char* name){
	mkfile(head, name)->dir = false;
}