#include "semaphore.h"
#include "memman.h"
#include "scheduler.h"
#include <queuePidADT.h>
#include <io.h>

static sem_t *semaphores[MAX_SEMAPHORES] = {NULL};

static int find_semaphore(uint64_t id) {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphores[i] != NULL && semaphores[i]->id == id) {
            return i;
        }
    }
    return -1;
}

static void lock_semaphore(sem_t *sem) {
    while (asm_xchg(&(sem->mutex), 1)) {
        uint64_t current_pid = get_current_pid();
        enqueue(sem->mutex_list, current_pid);
        block_process(current_pid);
        // yield();
    }
}

static void unlock_semaphore(sem_t *sem) {
    int64_t first_pid = peek(sem->mutex_list);
    if (first_pid != -1) {
        dequeue(sem->mutex_list);  
        unblock_process(first_pid);
    }
    sem->mutex = 0;
}

int32_t sem_open(uint64_t id, uint64_t initial_value) {
    if (find_semaphore(id) != -1) {
        return 0;
    }
    for(int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphores[i] == NULL) {
            semaphores[i] = (sem_t *)mem_alloc(sizeof(sem_t));
            semaphores[i]->id = id;
            semaphores[i]->value = initial_value;
            semaphores[i]->mutex = 0;  
            semaphores[i]->waiting_list = createQueue();
            semaphores[i]->mutex_list = createQueue();
            ker_write("creado semaforo con exito\n");
            return 0;
        }
    }
    return -1;
}

int32_t sem_create(uint64_t initial_value) {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphores[i] == NULL) {
            uint64_t id = 1;
            while (find_semaphore(id) != -1) {
                id++;
            }
            if (sem_open(id, initial_value) == 0) {
                return id;
            }
            return -1;
        }
    }
    return -1;
}

void sem_wait(uint64_t id) {
    int idx = find_semaphore(id);
    if (idx == -1) {
        return;
    }
    
    sem_t *sem = semaphores[idx];
    lock_semaphore(sem);
    
    while (sem->value == 0) {
        uint64_t current_pid = get_current_pid();
        enqueue(sem->waiting_list, current_pid);
        
        unlock_semaphore(sem);
        block_process(current_pid);
        //yield();
        lock_semaphore(sem);
        //ker_write("b");
    }
    
    sem->value--;
    unlock_semaphore(sem);
}

void sem_post(uint64_t id) {
    int idx = find_semaphore(id);
    if (idx == -1) {
        return;
    }
    
    sem_t *sem = semaphores[idx];
    lock_semaphore(sem);
    
    sem->value++;
    if (sem->value != 0) {
        int64_t waiting_pid = peek(sem->waiting_list);
        if (waiting_pid != -1) {
            dequeue(sem->waiting_list); 
            unblock_process(waiting_pid);
        }
    }
    
    unlock_semaphore(sem);
}

void sem_close(uint64_t id) {
    int idx = find_semaphore(id);
    if (idx == -1) {
        return;
    }
    
    destroyQueue(semaphores[idx]->waiting_list);
    destroyQueue(semaphores[idx]->mutex_list);
    mem_free(semaphores[idx]);
    semaphores[idx] = NULL;
}

void sem_cleanup_process(uint64_t pid) {
   
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphores[i] != NULL) {
           
            queuePIDADT new_waiting_list = createQueue();
            queuePIDADT new_mutex_list = createQueue();
            
         
            while (!isEmpty(semaphores[i]->waiting_list)) {
                int64_t current_pid = dequeue(semaphores[i]->waiting_list);
                if (current_pid != pid) {
                    enqueue(new_waiting_list, current_pid);
                }
            }
            
            
            while (!isEmpty(semaphores[i]->mutex_list)) {
                int64_t current_pid = dequeue(semaphores[i]->mutex_list);
                if (current_pid != pid) {
                    enqueue(new_mutex_list, current_pid);
                }
            }
            
        
            destroyQueue(semaphores[i]->waiting_list);
            destroyQueue(semaphores[i]->mutex_list);
            semaphores[i]->waiting_list = new_waiting_list;
            semaphores[i]->mutex_list = new_mutex_list;
        }
    }
}