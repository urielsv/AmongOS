#include <buddy_memman.h>
#include <stdint.h>
#include <definitions.h>
#include <stddef.h>
#include <stdlib.h>
#include <io.h>

#define MIN_BLOCK_ORDER 8 // 2^8 = 256 bytes
#define MAX_BLOCK_ORDER 19 // 2^19 = 512KB = 524288 bytes
#define MAX_ORDER (MAX_BLOCK_ORDER - MIN_BLOCK_ORDER)
#define MIN_ALLOC_SIZE (1ULL << MIN_BLOCK_ORDER)  


typedef struct block_node_t {
    struct block_node_t* next;
    uint8_t order;                          
    bool is_free;
} block_node_t;

typedef struct buddy_allocator_cdt {
    block_node_t* free_lists[MAX_ORDER + 1];
    void* start_addr;                        
    uint64_t total_size;                    
    uint64_t free_memory;                    
}buddy_allocator_cdt ;

static uintptr_t ALLOCATOR_ADDRESS = -1;

// Helper function to get buddy address
static void* get_buddy_address(buddy_allocator_adt allocator, void* addr, uint8_t order) {
    uint64_t block_size = 1ULL << (order + MIN_BLOCK_ORDER);
    uint64_t relative_addr = (uint64_t)addr - (uint64_t)allocator->start_addr;
    uint64_t buddy_relative_addr = relative_addr ^ block_size;
    return (void*)((uint64_t)allocator->start_addr + buddy_relative_addr);
}

// Split a block into two buddies
static void split_block(buddy_allocator_adt allocator, block_node_t* block, uint8_t target_order) {
    while (block->order > target_order) {
        uint8_t curr_order = block->order;
        block->order = curr_order - 1;

        // Calculate buddy address
        uint64_t block_size = 1ULL << (curr_order - 1 + MIN_BLOCK_ORDER);
        block_node_t* buddy = (block_node_t*)((uint8_t*)block + block_size);
        
        // Initialize buddy
        buddy->order = curr_order - 1;
        buddy->is_free = true;
        buddy->next = allocator->free_lists[curr_order - 1];
        allocator->free_lists[curr_order - 1] = buddy;
    }
}

// Round up to 2^n
static uint8_t size_to_order(uint64_t size) {
    size = size - 1;
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    size |= size >> 32;
    size += 1;

    // Calculate order based on the power of 2 size
    uint8_t order = 0;
    uint64_t min_size = MIN_ALLOC_SIZE;
    
    while (min_size < size && order < MAX_ORDER) {
        min_size <<= 1;
        order++;
    }
    
    return order;
}

// Try to merge with buddy
static void try_merge(buddy_allocator_adt allocator, block_node_t* block) {
    while (block->order < MAX_ORDER) {
        block_node_t* buddy = (block_node_t*)get_buddy_address(allocator, block, block->order);

        if ((void*)buddy < allocator->start_addr ||
            (void*)buddy >= (void*)((uint8_t*)allocator->start_addr + allocator->total_size) ||
            !buddy->is_free ||
            buddy->order != block->order) {
            break;
        }

        // Remove buddy from its free list
        block_node_t** curr = &allocator->free_lists[block->order];
        while (*curr && *curr != buddy) {
            curr = &(*curr)->next;
        }
        if (*curr) {
            *curr = buddy->next;
        }

        // Merge blocks
        if ((uint64_t)block > (uint64_t)buddy) {
            block_node_t* temp = block;
            block = buddy;
            buddy = temp;
        }
        block->order++;
    }

    // Add merged block to free list
    block->next = allocator->free_lists[block->order];
    allocator->free_lists[block->order] = block;
}

// Constructor implementation
void b_init(void* start_addr, uint64_t total_size) {

    if (!start_addr || total_size == 0) {
        return;
    }
    if (((uintptr_t)start_addr & 0xFFF) != 0) {
        return;
    }
    if (total_size < (sizeof(buddy_allocator_adt) + MIN_ALLOC_SIZE)) {
        return;
    }
    memset(start_addr, 0, sizeof(buddy_allocator_adt));
    buddy_allocator_adt allocator = (buddy_allocator_adt) start_addr;
    
    uintptr_t memory_start = (uintptr_t)start_addr + sizeof(buddy_allocator_adt);
    memory_start = (memory_start + 0xFFF) & ~0xFFF; // Round up to next page boundary
    
    allocator->start_addr = (void*)memory_start;
    allocator->total_size = total_size - (memory_start - (uintptr_t)start_addr);
    allocator->free_memory = allocator->total_size;

    // Initialize free lists
    for (int i = 0; i <= MAX_ORDER; i++) {
        allocator->free_lists[i] = NULL;
    }

    // Calculate maximum possible order for the available space
    uint64_t max_possible_size = 1ULL << (MAX_ORDER + MIN_BLOCK_ORDER);
    uint8_t initial_order = MAX_ORDER;
    
    while (initial_order > 0 && (1ULL << (initial_order + MIN_BLOCK_ORDER)) > allocator->total_size) {
        initial_order--;
    }

    // Create and initialize the initial block
    block_node_t* initial_block = (block_node_t*)allocator->start_addr;
    if ((uintptr_t)initial_block & 0xFFF) {
        // If somehow we still got misaligned, abort
        return;
    }

    // Initialize the block
    initial_block->next = NULL;
    initial_block->order = initial_order;
    initial_block->is_free = true;

    // Add to appropriate free list
    allocator->free_lists[initial_order] = initial_block;

    // Set the global allocator address only after successful initialization
    ALLOCATOR_ADDRESS = (uintptr_t)start_addr;
}

// Destructor implementation
void b_destroy(buddy_allocator_adt allocator) {
    // In a bare metal OS, we might not need to do anything here
    // since we're managing physical memory directly
    (void)allocator;
}

// Allocation implementation
void* b_alloc(size_t size) {
    buddy_allocator_adt allocator = b_get_allocator();
    if (size == 0) {
        return NULL;
    }

    // Añadir el tamaño de la estructura de bloque
    size += sizeof(block_node_t);

    if (size < MIN_ALLOC_SIZE) {
        size = MIN_ALLOC_SIZE;
    }

    uint8_t order = size_to_order(size);

    // Buscar un bloque utilizable
    uint8_t current_order;
    for (current_order = order; current_order <= MAX_ORDER; current_order++) {
        if (allocator->free_lists[current_order]) {
            break;
        }
    }

    if (current_order > MAX_ORDER) {
        return NULL;
    }

    block_node_t* block = allocator->free_lists[current_order];
    allocator->free_lists[current_order] = block->next;

    allocator->free_memory -= (1ULL << (order + MIN_BLOCK_ORDER));

    if (current_order > order) {
        split_block(allocator, block, order);
    }

    block->is_free = false;
    
    return (void*)((uint8_t*)block + sizeof(block_node_t));
}


// Free implementation
void b_free(void* addr) {

    buddy_allocator_adt allocator = b_get_allocator();

    if (!addr || addr < allocator->start_addr || 
        addr >= (void*)((uint8_t*)allocator->start_addr + allocator->total_size)) {
        return;
    }

    block_node_t* block = (block_node_t*)addr;
    block->is_free = true;

    // Update free memory counter
    allocator->free_memory += (1ULL << (block->order + MIN_BLOCK_ORDER));

    // Try to merge with buddy
    try_merge(allocator, block);
}

// Get block size implementation
uint64_t b_get_block_size(void* addr) {
    if (!addr) return 0;
    block_node_t* block = (block_node_t*)addr;
    return 1ULL << (block->order + MIN_BLOCK_ORDER);
}

// Get free memory implementation
uint64_t b_get_free_memory(buddy_allocator_adt allocator) {
    return allocator->free_memory;
}

buddy_allocator_adt b_get_allocator() {
    if (ALLOCATOR_ADDRESS == -1) {
        return NULL;
    }
    return (buddy_allocator_adt) ALLOCATOR_ADDRESS;
}