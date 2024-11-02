#ifndef BUDDY_MEMMAN_H
#define BUDDY_MEMMAN_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


// Forward declaration of the buddy allocator structure
typedef struct buddy_allocator_cdt buddy_allocator_adt;

// Constructor - Creates a new buddy allocator
void  b_init(void* start_addr, uint64_t total_size);

// Destructor - Cleans up the buddy allocator
void b_destroy(buddy_allocator_adt* allocator);

// Allocate memory of given order
void* b_alloc(size_t size);

// Free a previously allocated block
void b_free(void* ptr);

// Get the size of an allocated block
uint64_t b_get_block_size(void* addr);

// Get total free memory
uint64_t b_get_free_memory(buddy_allocator_adt* allocator);

// Getter of the allocator
buddy_allocator_adt* b_get_allocator();


#endif