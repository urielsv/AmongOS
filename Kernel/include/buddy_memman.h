#ifndef BUDDY_MEMMAN_H
#define BUDDY_MEMMAN_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct buddy_allocator_cdt * buddy_allocator_adt;
void  b_init(void* start_addr, uint64_t total_size);
void* b_alloc(size_t size);
void b_free(void* ptr);
uint64_t b_get_block_size(void* addr);
uint64_t b_get_free_memory(buddy_allocator_adt allocator);
buddy_allocator_adt b_get_allocator();


#endif