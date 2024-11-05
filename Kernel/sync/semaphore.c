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
    int aux;
    // while (aux = asm_xchg((sem->mutex), 1)==1) {
    //    // ker_write("waiting mutex to change value");
    //     int64_t current_pid = get_current_pid();
    //     enqueue(sem->mutex_list, current_pid);
    //     block_process(current_pid);
    //     ker_write("proceso bloquead en lock id:");
    //     print_number(current_pid);
    //     ker_write("\n");
    //     // yield();
    // }
    acquire(&sem->mutex);
    // sem->mutex--;
}

static void unlock_semaphore(sem_t *sem) {
    // int64_t first_pid = peek(sem->mutex_list);
    // if (first_pid != -1) {
    //     dequeue(sem->mutex_list);  
    //     unblock_process(first_pid);
    //     ker_write("proceso desbloquead en lock id:");
    //     print_number(first_pid);
    //     ker_write("\n");
    // }
    // sem->mutex ++;
    release(&sem->mutex);
}

int32_t sem_open(int64_t id, int64_t initial_value) {

    int find =find_semaphore(id);
    if ( find != -1) {
        return find;
    }
   // for(int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphores[id] == NULL) {
            semaphores[id] = (sem_t *)mem_alloc(sizeof(sem_t));
            semaphores[id]->id = id;
            semaphores[id]->value = initial_value;
            semaphores[id]->mutex = 1;  
            semaphores[id]->waiting_list = createQueue();
            semaphores[id]->mutex_list = createQueue();
            ker_write("creado semaforo con exito\n");
            return 0;
        }
    // }
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

    // ker_write("semaphore mutex before lock :");
    // print_number(sem->mutex);
    // ker_write("\n");

    lock_semaphore(sem);

    // ker_write("semaphore mutex after lock :");
    // print_number(sem->mutex);
    // ker_write("\n");
    
    // ker_write("semaphore value before semWait :");
    // print_number(sem->value);
    // ker_write("\n");

    while (sem->value == 0) {
        int64_t current_pid = get_current_pid();
        enqueue(sem->waiting_list, current_pid);
        
        unlock_semaphore(sem);
        // ker_write("proceso bloqueado en semWait id:");
        // print_number(current_pid);
        // ker_write("\n");
        block_process(current_pid);
        //yield();
        lock_semaphore(sem);
        //ker_write("b");
    }


    
    sem->value--;

    // ker_write("sem value after semWait:");
    // print_number(sem->value);
    // ker_write("\n");

    // ker_write("semaphore mutex before unlock :");
    // print_number(sem->mutex);
    // ker_write("\n");

    unlock_semaphore(sem);

    // ker_write("semaphore mutex after unlock :");
    // print_number(sem->mutex);
    // ker_write("\n");
}

void sem_post(int64_t id) {
    int idx = find_semaphore(id);
    if (idx == -1) {
        return;
    }
    
    sem_t *sem = semaphores[idx];

    // ker_write("semaphore mutex before lock :");
    // print_number(sem->mutex);
    // ker_write("\n");

    lock_semaphore(sem);

    // ker_write("semaphore mutex after lock :");
    // print_number(sem->mutex);
    // ker_write("\n");

    // ker_write("semaphore value before semPost :");
    // print_number(sem->value);
    // ker_write("\n");
    
    sem->value++;

    // ker_write("semaphore value after semPost :");
    // print_number(sem->value);
    // ker_write("\n");

    if (sem->value != 0) {
        int64_t waiting_pid = peek(sem->waiting_list);
        if (waiting_pid != -1) {
            dequeue(sem->waiting_list); 
            unblock_process(waiting_pid);
            // ker_write("proceso desbloquead en semPost id:");
            // print_number(waiting_pid);
            // ker_write("\n");
        }
    }
    
    // ker_write("semaphore mutex before unlock :");
    // print_number(sem->mutex);
    // ker_write("\n");

    unlock_semaphore(sem);

    // ker_write("semaphore mutex after unlock :");
    // print_number(sem->mutex);
    // ker_write("\n");

}

void sem_close(int64_t id) {
    int idx = find_semaphore(id);
    if (idx == -1) {
        return;
    }
    
    destroyQueue(semaphores[idx]->waiting_list);
    destroyQueue(semaphores[idx]->mutex_list);
    mem_free(semaphores[idx]);
    semaphores[idx] = NULL;
}

void sem_cleanup_process(int64_t pid) {
   
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

void int_to_string(int num, char *str) {
    int i = 0;
    int is_negative = 0;

    // Handle 0 explicitly, since it is a special case
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    // Handle negative numbers
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    // Process individual digits
    while (num != 0) {
        str[i++] = (num % 10) + '0'; // Convert digit to character
        num /= 10;
    }

    // If the number is negative, append '-'
    if (is_negative) {
        str[i++] = '-';
    }

    str[i] = '\0'; // Null-terminate the string

    // Reverse the string
    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}

void print_number(int number) {
    char buffer[20]; // Buffer to hold the string representation of the number
    int_to_string(number, buffer); // Convert integer to string
    ker_write(buffer); // Print the number
}


