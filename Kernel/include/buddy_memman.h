// buddy_memman.h
#ifndef BUDDY_MEMMAN_H
#define BUDDY_MEMMAN_H

#include <stdint.h>

#define MAX_ORDER 32
#define MIN 4
#define taken 0
#define free 1

typedef struct block_t {
    uint8_t order;
    uint8_t state;
    struct block_t *prev;
    struct block_t *next;
} block_t;

typedef struct {
    void *base;
    uint8_t max;
    block_t *free_lists[MAX_ORDER];
    uint64_t total_mem;
    uint64_t used_mem;
} buddy_t;

void b_init(void *initial_addr, uint64_t size);
void *b_alloc(uint64_t size);
void b_free(void *ptr);

#endif