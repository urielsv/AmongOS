#ifndef semaphore_h
#define semaphore_h

#include <stdint.h>
#include <linked_list_adt.h>
#include <queue_pid_adt.h>

#define max_semaphores (1<<6)

typedef struct {
    int64_t id;
    int64_t value;
    int8_t mutex;
    queue_pid_adt waiting_list;  
} sem_t;

int32_t sem_open(int64_t id, int64_t initial_value);
int32_t sem_create(int64_t initial_value);
void sem_close(int64_t id);
void sem_wait(int64_t id);
void sem_post(int64_t id);
void sem_cleanup_process(int64_t pid);
void remove_from_all_semaphores(uint32_t pid);

extern void acquire(int8_t *lock);
extern void release(int8_t *lock);



#endif
