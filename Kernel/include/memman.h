#ifndef MEMMAN_H
#define MEMMAN_H

#include <stddef.h>
#include <stdint.h>

void b_init(void *start_addr, size_t size);
void *b_alloc(size_t size);
void b_free(void *ptr);
size_t * mem_info();

#endif