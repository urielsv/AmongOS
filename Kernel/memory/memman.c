// this is a personal academic project. dear pvs-studio, please check it.
// pvs-studio static code analyzer for c, c++ and c#: http://www.viva64.com
#if defined MEMMAN

#include <stddef.h>
#include <stdint.h>
#include "memman.h"
#include <lib.h>

#define mem_block_size 4096

static uint64_t mem_size;


typedef struct mem_node{
    void *start_addr;
    size_t size;
    void *next;
    uint8_t is_free;
} mem_node;

typedef struct mem_manager {
    void *heap_start_addr;
    void *heap_end_addr;
    mem_node *mem_list;
    size_t total_memory;  
    size_t used_memory;   
    size_t free_memory;  
} mem_manager;

mem_manager *memory_manager;

void b_init(void *start_addr, size_t size) {
    mem_size = size;
    memory_manager = (mem_manager *)start_addr;
    memory_manager->heap_start_addr = start_addr + sizeof(mem_manager);
    memory_manager->heap_end_addr = start_addr + size;
    memory_manager->mem_list = NULL;
    memory_manager->total_memory = mem_size;  
    memory_manager->free_memory = mem_size;   
    memory_manager->used_memory = memory_manager->total_memory - memory_manager->free_memory;       
}

static mem_node create_block(void *start_addr, size_t size) {
    mem_node new_node;
    new_node.start_addr = start_addr;
    new_node.size = size;
    new_node.is_free = 0;
    new_node.next = NULL;
    return new_node;
}

void *b_alloc(size_t size) {
    if (size == 0)
        return NULL;

    mem_node *current_node = memory_manager->mem_list;
    mem_node *prev_node = NULL;

    while (current_node != NULL) {
        if (current_node->is_free && current_node->size >= size) {
            current_node->is_free = 0;
            memory_manager->used_memory += current_node->size; 
            memory_manager->free_memory -= current_node->size;
            return current_node->start_addr;
        }
        prev_node = current_node;
        current_node = current_node->next;
    }

    void *new_block_addr = (prev_node == NULL) ? memory_manager->heap_start_addr : prev_node->start_addr + prev_node->size;
    if (new_block_addr + size + sizeof(mem_node) > memory_manager->heap_end_addr) {
        return NULL; // no hay suficiente memoria
    }

    mem_node *new_node = (mem_node *)new_block_addr;
    new_node->start_addr = new_block_addr + sizeof(mem_node); 
    new_node->size = size;
    new_node->is_free = 0;
    new_node->next = NULL;

    if (prev_node == NULL) {
        memory_manager->mem_list = new_node;
    } else {
        prev_node->next = new_node;
    }

    memory_manager->used_memory += size; 
    memory_manager->free_memory -= size;

    return new_node->start_addr;
    //nunca llega aca 
     if (prev_node->start_addr + prev_node->size + size + sizeof(mem_node) > memory_manager->heap_end_addr) {
        return (void*)0;
    }
    mem_node node = create_block(current_node->start_addr + current_node->size, size);
    current_node->next = &node;
    return node.start_addr;
}

void b_free(void *ptr) {
    mem_node *current_node = memory_manager->mem_list;
    while (current_node != NULL) {
        if (current_node->start_addr == ptr) {
            current_node->is_free = 1;
            memory_manager->used_memory -= current_node->size; 
            memory_manager->free_memory += current_node->size;
            mem_info();
            return;
        }
        current_node = current_node->next;
    }
}

size_t * mem_info() {
    size_t *info = (size_t *)b_alloc(3 * sizeof(size_t)); 
    if (info == NULL) {
        return NULL; 
    }
    info[0] = memory_manager->total_memory;
    info[1] = memory_manager->used_memory;
    info[2] = memory_manager->free_memory;
    return info; 
}

#endif