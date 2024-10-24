// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stddef.h>
#include <stdint.h>
#include "memman.h"

#define MEM_BLOCK_SIZE 4096

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
} mem_manager;

mem_manager *memory_manager;

void mem_init(void *start_addr, size_t size) {
    mem_size = size;
    memory_manager = (mem_manager *)start_addr;
    memory_manager->heap_start_addr = start_addr + sizeof(mem_manager);
    memory_manager->heap_end_addr = start_addr + size;
    //memory_manager->mem_list = (mem_node *)(start_addr + sizeof(mem_manager));
    memory_manager->mem_list = NULL;
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
    if (size == 0)
        return NULL;

    mem_node *current_node = memory_manager->mem_list;
    mem_node *prev_node = NULL;

    // Buscar un bloque libre adecuado
    while (current_node != NULL) {
        if (current_node->is_free && current_node->size >= size) {
            current_node->is_free = 0;
            return current_node->start_addr;
        }
        prev_node = current_node;
        current_node = current_node->next;
    }

    // Crear un nuevo bloque si no se encontró uno adecuado
    void *new_block_addr = (prev_node == NULL) ? memory_manager->heap_start_addr : prev_node->start_addr + prev_node->size;
    if (new_block_addr + size + sizeof(mem_node) > memory_manager->heap_end_addr) {
        return NULL; // No hay suficiente memoria
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

    return new_node->start_addr;
     if (prev_node->start_addr + prev_node->size + size + sizeof(mem_node) > memory_manager->heap_end_addr) {
        return (void*)0;
    }
    mem_node node = create_block(current_node->start_addr + current_node->size, size);
    current_node->next = &node;
    return node.start_addr;
}



void mem_free(void *ptr) {
    mem_node *current_node = memory_manager->mem_list;
    while (current_node != NULL) {
        if (current_node->start_addr == ptr) {
            current_node->is_free = 1;
            return;
        }
        current_node = current_node->next;
    }
}