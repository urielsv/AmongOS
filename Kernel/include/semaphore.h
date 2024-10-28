#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdint.h>
#include <linkedListADT.h>

#define MAX_SEMAPHORES (1<<6)

typedef struct {
    uint64_t id;
    uint64_t value;
    uint8_t mutex;
    linkedListADT waiting_processes;  // List of blocked processes waiting on semaphore
    linkedListADT mutex_processes;    // List of processes waiting for mutex
} semaphore_t;

// Internal functions declarations
int32_t sem_open(uint64_t id, uint64_t initial_value);
void sem_close(uint64_t id);
int32_t sem_create(uint64_t initial_value);
void sem_wait(uint64_t id);
void sem_post(uint64_t id);
void sem_cleanup_process(uint64_t pid);

extern int asm_xchg(uint8_t *lock, int value);



#endif
