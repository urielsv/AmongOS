#ifndef MEMMAN_H
#define MEMMAN_H

#include <stddef.h>

void mem_init(void *start_addr, size_t size);
void *mem_alloc(size_t size);
void mem_free(void *ptr);
size_t * mem_info();

#endif