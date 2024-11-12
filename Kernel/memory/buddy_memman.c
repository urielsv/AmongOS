#include <buddy_memman.h>
#include <stddef.h>

static buddy_t *buddy_addr = NULL;

static uint8_t get_order(uint64_t size);
static block_t *new_block_t(void *address, uint8_t order, block_t *next);
static void free_lists_remove(buddy_t *buddy, block_t *header);
static void split(buddy_t *buddy, uint8_t idx);
static block_t *merge_block(buddy_t *buddy, block_t *block, block_t *buddy_block);
static inline void *hide_block_t(block_t *block);
static inline block_t *show_block_t(void *ptr);
static inline uint64_t block_size(uint8_t order);
static inline block_t *buddy_of(block_t *block);
static buddy_t *getBuddy(void);

static buddy_t *getBuddy(void) {
    return buddy_addr;
}

void b_init(void *initial_addr, uint64_t size) {
    buddy_addr = (buddy_t *)initial_addr;
    buddy_t *buddy = getBuddy();
    
    buddy->base = (void *)((uintptr_t)initial_addr + sizeof(buddy_t));
    buddy->max = get_order(size);
    buddy->total_mem = size;
    buddy->used_mem = 0;
    
    // Initialize free lists
    for(int i = 0; i < MAX_ORDER; i++) {
        buddy->free_lists[i] = NULL;
    }
    
    buddy->free_lists[buddy->max - 1] = new_block_t(buddy->base, buddy->max, NULL);
}

void *b_alloc(uint64_t size) {
    buddy_t *buddy = getBuddy();
    uint8_t order = get_order(size + sizeof(block_t));
    
    if (order < MIN - 1) {
        order = MIN - 1;
    }
    if (order >= buddy->max) {
        return NULL;
    }

    uint8_t to_split = order;
    while (1) {
        if (to_split >= buddy->max) {
            return NULL;
        }
        if (buddy->free_lists[to_split] != NULL) {
            break;
        }
        to_split++;
    }

    while (to_split > order) {
        split(buddy, to_split);
        to_split--;
    }

    block_t *block = buddy->free_lists[order];
    free_lists_remove(buddy, block);
    block->state = taken;
    buddy->used_mem += block_size(block->order);
    return hide_block_t(block);
}

void b_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    
    buddy_t *buddy = getBuddy();
    block_t *block = show_block_t(ptr);
    block->state = free;
    buddy->used_mem -= block_size(block->order);

    for (block_t *buddy_block = buddy_of(block); 
         buddy_block->state == free && buddy_block->order == block->order && block->order < buddy->max; 
         buddy_block = buddy_of(block)) {
        block = merge_block(buddy, block, buddy_block);
    }

    buddy->free_lists[block->order - 1] = new_block_t(block, block->order, buddy->free_lists[block->order - 1]);
}

static uint8_t get_order(uint64_t size) {
    uint8_t i = 0;
    while (size >>= 1) {
        i++;
    }
    return i;
}

static block_t *new_block_t(void *address, uint8_t order, block_t *next) {
    block_t *header = (block_t *)address;
    header->order = order;
    header->state = free;
    header->prev = NULL;
    header->next = next;
    if (next != NULL) {
        next->prev = header;
    }
    return header;
}

static void free_lists_remove(buddy_t *buddy, block_t *header) {
    if (header->prev == NULL) {
        buddy->free_lists[header->order - 1] = header->next;
    } else {
        header->prev->next = header->next;
    }
    if (header->next != NULL) {
        header->next->prev = header->prev;
    }
}

static void split(buddy_t *buddy, uint8_t idx) {
    block_t *block = buddy->free_lists[idx];
    free_lists_remove(buddy, block);
    block_t *buddy_block = (block_t *)((void *)block + (1 << idx));
    new_block_t(buddy_block, idx, buddy->free_lists[idx - 1]);
    buddy->free_lists[idx - 1] = new_block_t(block, idx, buddy_block);
}

static block_t *merge_block(buddy_t *buddy, block_t *block, block_t *buddy_block) {
    free_lists_remove(buddy, buddy_block);
    block_t *block_start = block < buddy_block ? block : buddy_block;
    block_start->order++;
    return block_start;
}

static inline void *hide_block_t(block_t *block) {
    return (void *)((uintptr_t)block + sizeof(block_t));
}

static inline block_t *show_block_t(void *ptr) {
    return (block_t *)((uintptr_t)ptr - sizeof(block_t));
}

static inline uint64_t block_size(uint8_t order) {
    return 1ULL << order;
}

static inline block_t *buddy_of(block_t *block) {
    buddy_t *buddy = getBuddy();
    uintptr_t block_addr = (uintptr_t)block;
    uintptr_t relative_addr = block_addr - (uintptr_t)buddy->base;
    uintptr_t buddy_relative_addr = relative_addr ^ (1ULL << block->order);
    return (block_t *)((uintptr_t)buddy->base + buddy_relative_addr);
}