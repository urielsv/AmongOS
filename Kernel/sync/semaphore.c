#include "semaphore.h"
#include "memman.h"
#include "scheduler.h"
#include <queuePidADT.h>
#include <io.h>

static sem_t *semaphores[MAX_SEMAPHORES] = {NULL};

void print_number(int number);

static int find_semaphore(int64_t id) {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphores[i] != NULL && semaphores[i]->id == id) {
            return i;
        }
    }
    return -1;
}

static void lock_semaphore(sem_t *sem) {
    acquire(&sem->mutex);
}

static void unlock_semaphore(sem_t *sem) {
    release(&sem->mutex);
}

int32_t sem_open(int64_t id, int64_t initial_value) {

    int find = find_semaphore(id);
    if ( find != -1) {
        return find;
    }
        if (semaphores[id] == NULL) {
            semaphores[id] = (sem_t *)mem_alloc(sizeof(sem_t));
            semaphores[id]->id = id;
            semaphores[id]->value = initial_value;
            semaphores[id]->mutex = 1;  
            semaphores[id]->waiting_list = createQueue();
            return 0;
        }
    return -1;
}

int32_t sem_create(int64_t initial_value) {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphores[i] == NULL) {
            int64_t id = 1;
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

void sem_wait(int64_t id) {
    int idx = find_semaphore(id);
    if (idx == -1) {
        return;
    }
    
    sem_t *sem = semaphores[idx];

    lock_semaphore(sem);


    while (sem->value == 0) {
        int64_t current_pid = get_current_pid();
        enqueue(sem->waiting_list, current_pid);
        
        unlock_semaphore(sem);
        block_process(current_pid);
        yield();
        lock_semaphore(sem);
    }

    sem->value--;

    unlock_semaphore(sem);

}

void sem_post(int64_t id) {
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
            yield();
        }
    }
    

    unlock_semaphore(sem);
    yield();

}

void sem_close(int64_t id) {
    int idx = find_semaphore(id);
    if (idx == -1) {
        return;
    }
    
    destroyQueue(semaphores[idx]->waiting_list);
    mem_free(semaphores[idx]);
    semaphores[idx] = NULL;
}

void sem_cleanup_process(int64_t pid) {
   
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphores[i] != NULL) {
           
            queuePIDADT new_waiting_list = createQueue();            
         
            while (!isEmpty(semaphores[i]->waiting_list)) {
                int64_t current_pid = dequeue(semaphores[i]->waiting_list);
                if (current_pid != pid) {
                    enqueue(new_waiting_list, current_pid);
                }
            }
            destroyQueue(semaphores[i]->waiting_list);
            semaphores[i]->waiting_list = new_waiting_list;
        }
    }
}

void remove_from_all_semaphores(uint32_t pid){
    for(int i = 0; i < MAX_SEMAPHORES; i++){
        if(semaphores[i] != NULL){
            dequeue_pid(semaphores[i]->waiting_list, pid);
        }
    }
}