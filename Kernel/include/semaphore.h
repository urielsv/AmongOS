#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdint.h>
#include <linkedListADT.h>
#include <queuePidADT.h>

#define MAX_SEMAPHORES (1<<6)

typedef struct {
    int64_t id;
    int64_t value;
    int8_t mutex;
    queuePIDADT waiting_list;  
    queuePIDADT mutex_list;
} sem_t;

int32_t sem_open(int64_t id, int64_t initial_value);
int32_t sem_create(int64_t initial_value);
void sem_close(int64_t id);
void sem_wait(int64_t id);
void sem_post(int64_t id);
void sem_cleanup_process(int64_t pid);

extern int asm_xchg(int8_t *lock, int value);
extern void acquire(int8_t *lock);
extern void release(int8_t *lock);



#endif
