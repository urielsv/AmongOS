#include "semaphore.h"
#include "memman.h"
#include "scheduler.h"
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
        uint64_t pid = get_current_pid();
        addNode(sem->mutex_list, (void *) ((uint64_t) pid));
        block_process(pid);
        yield();
    }
   
}

static void unlock_semaphore(sem_t *sem) {
    node_t * first = getFirstNode(sem->mutex_list);
    if (first != NULL) {
        uint64_t pid = (uint64_t) first->process;
        removeFirstNode(sem->mutex_list);
        unblock_process(pid);
    } else {
        sem->mutex = 0;
    }
    
}
    

int32_t sem_open(uint64_t id, uint64_t initial_value) {
   
    if (find_semaphore(id) != -1) {
        return 0;  
    }

   
        if (semaphores[id] == NULL) {
            semaphores[id] = (sem_t *)mem_alloc(sizeof(sem_t));
            semaphores[id]->id = id;
            semaphores[id]->value = initial_value;
            semaphores[id]->mutex = 0;  // Initialize to 0 (unlocked)
            semaphores[id]->waiting_list = createLinkedList();
            semaphores[id]->mutex_list = createLinkedList();
            ker_write("creado semaforo con exito\n");
            return 0;
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
        uint64_t pid = get_current_pid();
        addNode(sem->waiting_list, (void *) ((uint64_t) pid));
        unlock_semaphore(sem);
        block_process(pid);
        lock_semaphore(sem);
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
    if (sem->value > 0 ) {
        node_t *first = getFirstNode(sem->waiting_list);
        if (first != NULL) {
            uint64_t pid = (uint64_t)first->process;
            removeNode(sem->waiting_list, first->process);
            unblock_process(pid);
        }
    }
    unlock_semaphore(sem);
}

void sem_close(uint64_t id) {
    int idx = find_semaphore(id);
    if (idx == -1) {
        return;
    }

    destroyLinkedList(semaphores[idx]->waiting_list);
    destroyLinkedList(semaphores[idx]->mutex_list);
    mem_free(semaphores[idx]);
    semaphores[idx] = NULL;
}

void sem_cleanup_process(uint64_t pid) {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphores[i] != NULL) {
            removeNode(semaphores[i]->waiting_list, (void *) ((uint64_t) pid));
            removeNode(semaphores[i]->mutex_list, (void *) ((uint64_t) pid));
        }
    }
}