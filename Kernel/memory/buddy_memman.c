#include <buddy_memman.h>
#include <stdint.h>
#include <definitions.h>
#include <stddef.h>
#include <stdlib.h>
#include <io.h>
#include <lib.h>

#define MIN_BLOCK_ORDER 8 // 2^8 = 256 bytes
#define MAX_BLOCK_ORDER 19 // 2^19 = 512KB = 524288 bytes
#define MAX_ORDER (MAX_BLOCK_ORDER - MIN_BLOCK_ORDER+1)
#define MIN_ALLOC_SIZE (1ULL << MIN_BLOCK_ORDER)  


typedef struct block_node_t {
    struct block_node_t* prev;
    struct block_node_t* next;
    uint8_t order;                   
    bool is_free;
} block_node_t;

typedef struct buddy_allocator_cdt {
    block_node_t* free_lists[MAX_ORDER];
    void* start_addr;                        
    uint64_t total_size;                    
    uint64_t free_memory;                    
}buddy_allocator_cdt ;

static uintptr_t ALLOCATOR_ADDRESS = -1;
static void* get_buddy_address(buddy_allocator_adt allocator, void* addr, uint8_t order);
static void split_block(buddy_allocator_adt allocator, uint16_t idx, uint8_t target_order);
static uint8_t size_to_order(uint64_t size);
static void try_merge(buddy_allocator_adt allocator, block_node_t* block);

void b_init(void* start_addr, uint64_t total_size) {

    if (!start_addr || total_size == 0) {
        return;
    }

    buddy_allocator_adt allocator = (buddy_allocator_adt) start_addr;
    uintptr_t memory_start = (uintptr_t)start_addr + sizeof(buddy_allocator_cdt);
    memory_start = (memory_start + 0xFFF) & ~0xFFF;
    allocator->start_addr = (void*)memory_start;
    allocator->total_size = total_size - (memory_start - (uintptr_t)start_addr);
    allocator->free_memory = allocator->total_size;

    for (int i = 0; i < MAX_ORDER; i++) {
        allocator->free_lists[i] = NULL;
    }

    uint8_t initial_order = size_to_order(allocator->total_size);

    block_node_t* initial_block = (block_node_t*)allocator->start_addr;
    initial_block->next = NULL;
    initial_block->order = initial_order;
    initial_block->is_free = true;
    allocator->free_lists[initial_order-MIN_BLOCK_ORDER] = initial_block;

    ALLOCATOR_ADDRESS = (uintptr_t)start_addr;
}

void* b_alloc(size_t size) {

    buddy_allocator_adt allocator = b_get_allocator();
    ker_write("Trying to allocate memory\n");
    if (size == 0) {
        ker_write("Size is 0\n");
        return NULL;
    }
    size += sizeof(block_node_t);
    size = size < MIN_ALLOC_SIZE-1 ? MIN_ALLOC_SIZE-1 : size;

    ker_write("Size after min check: ");
    print_number(size);
    ker_write("\n");

    uint64_t order = size_to_order(size);
    ker_write("Order: ");
    print_number(order);
    ker_write("\n");
    uint8_t order_idx = order-MIN_BLOCK_ORDER;

    if (allocator->free_lists[order_idx] == NULL) {
        //ker_write("No block found in order index\n");
		uint8_t closest_idx = 0;
		for (uint8_t i = order_idx ; i < MAX_ORDER && !closest_idx; i++)
			if (allocator->free_lists[i] != NULL)
				closest_idx = i;
		if (closest_idx == 0){
            return NULL;
        }
			
		split_block(allocator, closest_idx, order_idx+MIN_BLOCK_ORDER);
	}

	block_node_t *block = allocator->free_lists[order_idx];
	allocator->free_lists[order_idx] = block->next;
	block->is_free = false;
	block->prev = NULL;
	block->next = NULL;
    block->order = order_idx + MIN_BLOCK_ORDER;

    allocator->free_memory -= (1ULL << (order_idx + MIN_BLOCK_ORDER));
    ker_write("free memory: ");
    print_number(allocator->free_memory);
    ker_write("\n");
	void *allocation = (void *) block + sizeof(block_node_t);
	return (void *) allocation;

}


// Helper function to get buddy address
static void* get_buddy_address(buddy_allocator_adt allocator, void* addr, uint8_t order) {
    uint64_t block_size = 1ULL << (order + MIN_BLOCK_ORDER);
    uint64_t relative_addr = (uint64_t)addr - (uint64_t)allocator->start_addr;
    uint64_t buddy_relative_addr = relative_addr ^ block_size;
    return (void*)((uint64_t)allocator->start_addr + buddy_relative_addr);
}

// Split a block into two buddies
static void split_block(buddy_allocator_adt allocator, uint16_t idx, uint8_t target_order) {

    // ker_write("Block order: ");
    // print_number(block->order);
    // ker_write("\n");
    // ker_write("Target order: ");
    // print_number(target_order);
    // ker_write("\n");

    //int i = 0;

    block_node_t * block = allocator->free_lists[idx];
    
    while (block->order > target_order) {

        // ker_write("Iteration: ");
        // print_number(i);
        // ker_write("\n");
        // i++;

        if (block->prev!=NULL) {
            block->prev->next = block->next;
        }
        else {
            allocator->free_lists[block->order-MIN_BLOCK_ORDER] = block->next;
        }
        if (block->next!=NULL) {
            block->next->prev = block->prev;
        }

        uint8_t curr_order = block->order;
        block->order = curr_order - 1;

        // Calculate buddy address
        uint64_t block_size = 1ULL << (block->order);
        block_node_t* buddy = (block_node_t*)((uint8_t*)block + block_size);
        
        // Initialize buddy
        buddy->order = curr_order - 1;

        // ker_write("Buddy order: ");
        // print_number(buddy->order);
        // ker_write("\n");    

        buddy->is_free = true;
        buddy->prev = NULL;
        buddy->next = allocator->free_lists[curr_order - 1 - MIN_BLOCK_ORDER];
        if (buddy->next!=NULL) {
            buddy->next->prev = buddy; 
        }
       
        allocator->free_lists[curr_order - 1 - MIN_BLOCK_ORDER] = block;
        block->order = curr_order - 1;

        block->is_free = true;
        block->next = buddy;
        block->prev = NULL;

    }
    //ker_write("End of split\n");
}

// Round up to 2^n
static uint8_t size_to_order(uint64_t size) {
    unsigned int count = 0;
	while (size /= 2){
        count++;
        if (size == 1){
            count++;
        }
    }

	return count;
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

    block->next = allocator->free_lists[block->order];
    allocator->free_lists[block->order] = block;
}

void b_free(void* addr) {

    buddy_allocator_adt allocator = b_get_allocator();

    if (!addr || addr < allocator->start_addr || 
        addr >= (void*)((uint8_t*)allocator->start_addr + allocator->total_size)) {
        ker_write("Invalid address\n");
        return;
    }
 
    block_node_t* block = (block_node_t*)((uint8_t*)addr - sizeof(block_node_t));
    block->is_free = true;

    //chequear desde aca. 
    // Update free memory counter
    allocator->free_memory += (1ULL << (block->order));
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
