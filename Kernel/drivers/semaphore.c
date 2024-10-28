#include <semaphore.h>
#include "../include/memman.h"
#include "../include/lib.h"
#include <string.h>
#include <linkedListADT.h>
#include <scheduler.h>
#include <process.h>



static semaphore_t *semaphores[MAX_SEMAPHORES] = {NULL};

static int find_semaphore(uint64_t id);
static void lock_semaphore(semaphore_t *sem);
static void unlock_semaphore(semaphore_t *sem);


// Initialize a new semaphore with given ID and value
int32_t sem_open(uint64_t id, uint64_t initial_value) {
    if (find_semaphore(id) != -1) {
        return -1;  // Semaphore already exists
    }

    // Find first free slot
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphores[i] == NULL) {
            semaphores[i] = (semaphore_t *)mem_alloc(sizeof(semaphore_t));
            if (semaphores[i] == NULL) {
                return -1;  // Memory allocation failed
            }

            semaphores[i]->id = id;
            semaphores[i]->value = initial_value;
            semaphores[i]->mutex = 0;
            semaphores[i]->waiting_processes = createLinkedList();
            semaphores[i]->mutex_processes = createLinkedList();

            return id;
        }
    }
    return -1;  // No free slots
}

// Create a new semaphore with automatic ID assignment
int32_t sem_create(uint64_t initial_value) {
    uint64_t new_id = 1;
    
    // Find first available ID
    while (find_semaphore(new_id) != -1) {
        new_id++;
        if (new_id == 0) {  // Overflow check
            return -1;
        }
    }

    return sem_open(new_id, initial_value);
}

// Close and free a semaphore
void sem_close(uint64_t id) {
    int idx = find_semaphore(id);
    if (idx == -1) {
        return;
    }

    // Free the lists
    while (getSize(semaphores[idx]->waiting_processes) > 0) {
        removeFirstNode(semaphores[idx]->waiting_processes);
    }
    while (getSize(semaphores[idx]->mutex_processes) > 0) {
        removeFirstNode(semaphores[idx]->mutex_processes);
    }

    // Free the semaphore structure
    mem_free(semaphores[idx]);
    semaphores[idx] = NULL;
}

// Wait operation on semaphore
void sem_wait(uint64_t id) {
    int idx = find_semaphore(id);
    if (idx == -1) {
        return;
    }

    semaphore_t *sem = semaphores[idx];
    lock_semaphore(sem);

    while (sem->value == 0) {
        process_t *current = (process_t *)mem_alloc(sizeof(process_t));
        current->pid = get_current_pid();
        
        // Add to waiting list and block
        addNode(sem->waiting_processes, current);
        unlock_semaphore(sem);
        block_process(current->pid);
        lock_semaphore(sem);
    }

    sem->value--;
    unlock_semaphore(sem);
}

// Post operation on semaphore
void sem_post(uint64_t id) {
    int idx = find_semaphore(id);
    if (idx == -1) {
        return;
    }

    semaphore_t *sem = semaphores[idx];
    lock_semaphore(sem);

    sem->value++;
    
    // If there are waiting processes, wake up the first one
    if (getSize(sem->waiting_processes) > 0) {
        node_t *first_node = getFirstNode(sem->waiting_processes);
        if (first_node != NULL) {
            process_t *process = (process_t *)first_node->process;
            removeFirstNode(sem->waiting_processes);
            unblock_process(process->pid);
            mem_free(process);
        }
    }

    unlock_semaphore(sem);
}

// Find semaphore by ID
static int find_semaphore(uint64_t id) {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphores[i] != NULL && semaphores[i]->id == id) {
            return i;
        }
    }
    return -1;
}

// Lock mutex using atomic exchange
static void lock_semaphore(semaphore_t *sem) {
    while (asm_xchg(&sem->mutex, 1)) {
        process_t *current = (process_t *)mem_alloc(sizeof(process_t));
        current->pid = get_current_pid();
        
        // Add to mutex waiting list and block
        addNode(sem->mutex_processes, current);
        block_process(current->pid);
    }
}

// Unlock mutex and wake up next waiting process
static void unlock_semaphore(semaphore_t *sem) {
    if (getSize(sem->mutex_processes) > 0) {
        node_t *first_node = getFirstNode(sem->mutex_processes);
        if (first_node != NULL) {
            process_t *process = (process_t *)first_node->process;
            removeFirstNode(sem->mutex_processes);
            unblock_process(process->pid);
            mem_free(process);
        }
    }
    sem->mutex = 0;
}

// Clean up process from all semaphores when it's killed
void sem_cleanup_process(uint64_t pid) {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphores[i] != NULL) {
            node_t *current = getFirstNode(semaphores[i]->waiting_processes);
            while (current != NULL) {
                process_t *process = (process_t *)current->process;
                if (process->pid == pid) {
                    removeNode(semaphores[i]->waiting_processes, process);
                    mem_free(process);
                    break;
                }
                current = current->next;
            }

            current = getFirstNode(semaphores[i]->mutex_processes);
            while (current != NULL) {
                process_t *process = (process_t *)current->process;
                if (process->pid == pid) {
                    removeNode(semaphores[i]->mutex_processes, process);
                    mem_free(process);
                    break;
                }
                current = current->next;
            }
        }
    }
}