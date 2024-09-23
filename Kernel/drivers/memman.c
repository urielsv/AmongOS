#include <stddef.h>
#include <stdint.h>
#include <memman.h>

#define MEM_BLOCK_SIZE 4096

static uint64_t mem_size;


typedef struct mem_node{
    void *start_addr;
    size_t size;
    mem_node *next;
    uint8_t is_free;
} mem_node;

typedef struct mem_manager {
    void *heap_start_addr;
    void *heap_end_addr;
    mem_node *mem_list;
} mem_manager;

mem_manager *memory_manager;

void mem_init(void *start_addr, size_t size) {
    mem_size = size;
    memory_manager = (mem_manager *)start_addr;
    memory_manager->heap_start_addr = start_addr + sizeof(mem_manager);
    memory_manager->heap_end_addr = start_addr + size;
    memory_manager->mem_list = (mem_node *)(start_addr + sizeof(mem_manager));
}

static mem_node create_block(void *start_addr, size_t size) {
    mem_node new_node;
    new_node.start_addr = start_addr;
    new_node.size = size;
    new_node.is_free = 0;
    new_node.next = NULL;
    return new_node;
}

void *mem_alloc(size_t size) {

    if(size == 0)
        return NULL;

    mem_node *current_node = memory_manager->mem_list;
    mem_node * prev_node = NULL;

    if(current_node == NULL)

    for (; current_node != NULL; prev_node = current_node, current_node = current_node->next) {
        if (current_node->is_free && current_node->size >= size) {
            mem_node new_node = create_block(current_node->start_addr, size);
            if(prev_node != NULL){
            prev_node->next = &new_node;
            }
            current_node->start_addr += size;
            current_node->size -= size;
            new_node.next = current_node;
            return new_node.start_addr;
        }
    }

    if (prev_node->start_addr + prev_node->size + size + sizeof(mem_node) > memory_manager->heap_end_addr) {
        return (void*)0;
    }
    mem_node new_node = create_block(current_node->start_addr + current_node->size, size);
    current_node->next = &new_node;
    return new_node.start_addr;

}



void mem_free(void *ptr) {
    mem_node *current_node = memory_manager->mem_list;
    for (; current_node != NULL; current_node = current_node->next) {
        if (current_node->start_addr == ptr) {
            current_node->is_free = 1;
            return;
        }
    }
    
}