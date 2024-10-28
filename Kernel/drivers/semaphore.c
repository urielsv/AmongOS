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
    while (asm_xchg(&sem->mutex, 1)) {
        uint64_t pid = get_current_pid();
        process_t *process = (process_t *)mem_alloc(sizeof(process_t));
        process->pid = pid;
        addNode(sem->mutex_list, process);
        block_process(pid);
    }
}

static void unlock_semaphore(sem_t *sem) {
    if (getSize(sem->mutex_list) > 0) {
        node_t *first = getFirstNode(sem->mutex_list);
        if (first != NULL) {
            process_t *process = (process_t *)first->process;
            uint64_t pid = process->pid;
            removeFirstNode(sem->mutex_list);
            mem_free(process);
            unblock_process(pid);
        }
    }
    sem->mutex = 0;
}

int32_t sem_open(uint64_t id, uint64_t initial_value) {
    if (find_semaphore(id) != -1) {
        return 0;  // Ya existe (éxito como en tu implementación)
    }

    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphores[i] == NULL) {
            semaphores[i] = (sem_t *)mem_alloc(sizeof(sem_t));
            semaphores[i]->id = id;
            semaphores[i]->value = initial_value;
            semaphores[i]->mutex = 0;
            semaphores[i]->waiting_list = createLinkedList();
            semaphores[i]->mutex_list = createLinkedList();
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
        uint64_t pid = get_current_pid();
        process_t *process = (process_t *)mem_alloc(sizeof(process_t));
        process->pid = pid;
        addNode(sem->waiting_list, process);
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
    
    if (getSize(sem->waiting_list) > 0) {
        node_t *first = getFirstNode(sem->waiting_list);
        if (first != NULL) {
            process_t *process = (process_t *)first->process;
            uint64_t pid = process->pid;
            removeFirstNode(sem->waiting_list);
            mem_free(process);
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

    // Liberar procesos en waiting_list
    while (getSize(semaphores[idx]->waiting_list) > 0) {
        node_t *first = getFirstNode(semaphores[idx]->waiting_list);
        if (first != NULL) {
            process_t *process = (process_t *)first->process;
            removeFirstNode(semaphores[idx]->waiting_list);
            mem_free(process);
        }
    }

    // Liberar procesos en mutex_list
    while (getSize(semaphores[idx]->mutex_list) > 0) {
        node_t *first = getFirstNode(semaphores[idx]->mutex_list);
        if (first != NULL) {
            process_t *process = (process_t *)first->process;
            removeFirstNode(semaphores[idx]->mutex_list);
            mem_free(process);
        }
    }

    destroyLinkedList(semaphores[idx]->waiting_list);
    destroyLinkedList(semaphores[idx]->mutex_list);
    mem_free(semaphores[idx]);
    semaphores[idx] = NULL;
}

void sem_cleanup_process(uint64_t pid) {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphores[i] != NULL) {
            // Buscar y eliminar de waiting_list
            node_t *current = getFirstNode(semaphores[i]->waiting_list);
            while (current != NULL) {
                process_t *process = (process_t *)current->process;
                if (process->pid == pid) {
                    removeNode(semaphores[i]->waiting_list, process);
                    mem_free(process);
                    break;
                }
                current = current->next;
            }

            // Buscar y eliminar de mutex_list
            current = getFirstNode(semaphores[i]->mutex_list);
            while (current != NULL) {
                process_t *process = (process_t *)current->process;
                if (process->pid == pid) {
                    removeNode(semaphores[i]->mutex_list, process);
                    mem_free(process);
                    break;
                }
                current = current->next;
            }
        }
    }
}