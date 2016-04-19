#include "initramfs.h"
#include "stdio.h"
#include "string.h"
#include "memory.h"
#include "multiboot.h"
#include "file.h"

static char* align4(char *addr) {
	return ((uint64_t) addr & 3) == 0 ? addr : addr + (4 - ((uint64_t) addr & 3));	
}

static uint32_t read_int(char *str) {
	uint32_t result = 0;
	for (int i = 0; i < 8; i++) {
		result <<= 4;
		if (str[i] >= '0' && str[i] <= '9')
			result += str[i] - '0';
		else if (str[i] >= 'A' && str[i] <= 'F')
			result += str[i] - 'A' + 10;
		else if (str[i] >= 'a' && str[i] <= 'f')
			result += str[i] - 'a' + 10;
	}
	return result;
}

static char *start_ramfs, *end_ramfs;

void setup_initramfs() {
	extern const uint32_t mboot_info;
 	multiboot_info_t *multiboot_info = (multiboot_info_t *) (uintptr_t) mboot_info;
 	if (!(multiboot_info->flags & (1<<3))) {
		return;
 	}
 	bool found = 0;
	multiboot_module_t *module = (multiboot_module_t*) (uintptr_t) multiboot_info->mods_addr;
 	for (uint32_t i = 0; i < multiboot_info->mods_count; i++, module++) {
 		if (module->mod_end - module->mod_start >= sizeof(cpio_header_t) && 
				!memcmp((void*) (uintptr_t) module->mod_start, CPIO_HEADER_MAGIC, 6)){
			found = 1;
 			break;
 		}
 	}
 	if(!found) {
		return;
 	}
	start_ramfs = (char*) (uintptr_t) module->mod_start;
	end_ramfs = (char*) (uintptr_t) module->mod_end;
	balloc_add_region((phys_t) start_ramfs, end_ramfs - start_ramfs);
	balloc_reserve_region((phys_t) start_ramfs, end_ramfs - start_ramfs);
}

void read_initramfs() {
	start_ramfs = va((phys_t) start_ramfs);
	end_ramfs = va((phys_t) end_ramfs);
	while (start_ramfs < end_ramfs) {
		start_ramfs = align4(start_ramfs);
		cpio_header_t *header = (cpio_header_t*) start_ramfs;
		uint32_t name_len = read_int(header->namesize);
		uint32_t file_len = read_int(header->filesize);
		start_ramfs += sizeof(cpio_header_t);
		if (!memcmp(start_ramfs, END_OF_ARCHIVE, strlen(END_OF_ARCHIVE))) {
			return;
		}
		
		char file_name[1<<10];
		file_name[0] = '/';
		for (uint32_t i = 0; i < name_len; i++, start_ramfs++) {
			file_name[i + 1] = *start_ramfs;
		} 

		file_name[name_len] = 0;
		struct file_t* file = NULL;
		if (S_ISDIR(read_int(header->mode))) {
			mkdir(file_name);
		} else {
			file = open(file_name);
		}
		start_ramfs = align4(start_ramfs);
		if (file) {
			write(file, 0, file_len, (char*)start_ramfs);
		}
		start_ramfs += file_len;
	}
}