#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <lib.h>


void b_init(void *initialAddress, uint64_t size);
void * b_alloc(uint64_t size);
void b_free(void * ptr);
void memory_info();



#endif