#include "../include/scheduler.h"
#include "../include/memman.h"
#include "../include/linkedListADT.h"
#include <io.h>
#include <stdlib.h>
#include <process.h>
#include <math.h>

#define IDLE_PID 0
#define DEFAULT_QUANTUM 3

#define CAPPED_PRIORITY(prio) (prio >= MAX_PRIORITY ? MAX_PRIORITY : prio)

typedef struct scheduler_cdt {
    node_t * processes[MAX_PROCESSES];
    linkedListADT blocked_process_list;
    linkedListADT process_list;
    int32_t current_pid;
    uint16_t next_unused_pid;
    uint16_t remaining_processes;
    int32_t current_quantum;
} scheduler_cdt;


void print_number2(int number);
// Inicializa el scheduler
scheduler_adt init_scheduler() {
    scheduler_adt scheduler = (scheduler_adt) SCHEDULER_ADDRESS;

    scheduler->process_list = createLinkedList();
    scheduler->blocked_process_list = createLinkedList();

    for (int i = 0; i < MAX_PROCESSES; i++) {
        scheduler->processes[i] = NULL;
    }

    scheduler->remaining_processes=0;
    scheduler->next_unused_pid = 0;
    scheduler->current_pid = -1;
    scheduler->current_quantum = DEFAULT_QUANTUM;

    return scheduler;
}

scheduler_adt getSchedulerADT() {
    return (scheduler_adt) SCHEDULER_ADDRESS;
}


// Selecciona el siguiente proceso listo para ejecutar
int32_t get_next_ready_pid() {
    

      scheduler_adt scheduler = getSchedulerADT();


    node_t * next_node = getFirstNode(scheduler->process_list);
    if (next_node != NULL) {
        return ((process_t *) next_node->process)->pid;
    }
    return IDLE_PID;  // Si no hay otro proceso listo, regresar al proceso idle

}


void* scheduler(void* stack_pointer) {
    // ker_write("cCOOOOONTEXT SWIIIIIITCHINNNNG\n");
    scheduler_adt scheduler = getSchedulerADT();
    scheduler->current_quantum--;

    process_t *current_process = NULL;

    // Case when there are no processes (only idle process)
    if (scheduler->current_pid == -1) {
        scheduler->current_pid = IDLE_PID;
        current_process = (process_t *) scheduler->processes[scheduler->current_pid]->process;
        current_process->state = RUNNING;  
        return current_process->stack_pointer;
    }

    // Case when there are no more processes to execute
    if (scheduler->remaining_processes == 0 || scheduler->current_quantum > 0) {
        return stack_pointer;
    }

    // Case when the current process is not the idle process
    node_t *current_process_node = scheduler->processes[scheduler->current_pid];
    current_process = (process_t *) current_process_node->process;
    current_process->stack_pointer = stack_pointer;


 
        switch (current_process->state) {
            
            case RUNNING:
                current_process->state = READY;
                swapToLast(scheduler->process_list, current_process);
                scheduler->current_pid = get_next_ready_pid();
                current_process = (process_t *) scheduler->processes[scheduler->current_pid]->process;
               
                //ker_write("RUNNING -> READY\n");
                break;

            case READY:
                current_process->state = RUNNING;
                current_process->stack_pointer = stack_pointer;
        
                //ker_write("READY -> RUNNING\n");
                break;

            case KILLED:
                scheduler->processes[scheduler->current_pid] = NULL;
                free_process(current_process);
                scheduler->current_pid = get_next_ready_pid();
                current_process = (process_t *) scheduler->processes[scheduler->current_pid]->process;
                //current_process->state = READY;
                scheduler->remaining_processes--;
                break;
            //aca nunca deberia entrar
            case BLOCKED:
                //ker_write("BLOCKED\n");
                scheduler->current_pid = get_next_ready_pid();
                current_process = (process_t *) scheduler->processes[scheduler->current_pid]->process;
                break;
            default:
               // ker_write("Other state");
                break;
        }
        //  ker_write("next process to run has id :");
        //         print_number(scheduler->current_pid);
        //         ker_write("\n");

        //          ker_write("blocked processes list pids: ");
        //         node_t *current_node = getFirstNode(scheduler->blocked_process_list); // Assuming you have a head pointer in your linked list structure
        //         while (current_node != NULL) {
        //             process_t *blocked_process = (process_t *)current_node->process;
        //             print_number2(blocked_process->pid); // Print the PID of the blocked process
        //             current_node = current_node->next; // Move to the next node
        //         }
                // ker_write("\n");

                //  ker_write("ready processes list pids: ");
                // current_node = getFirstNode(scheduler->process_list); // Assuming you have a head pointer in your linked list structure
                // while (current_node != NULL) {
                //     process_t *blocked_process = (process_t *)current_node->process;
                //     print_number2(blocked_process->pid); // Print the PID of the blocked process
                //     current_node = current_node->next; // Move to the next node
                // }
                // ker_write("\n");
        scheduler->current_quantum = DEFAULT_QUANTUM;
        return current_process->stack_pointer;
}



// Crear un nuevo proceso
int32_t create_process(Function code, char **args, int argc, char *name, uint8_t priority, uint8_t unkilliable) {

    priority = CAPPED_PRIORITY(priority);
    scheduler_adt scheduler = getSchedulerADT();
    //ker_write("Creating process\n");
    if (scheduler->remaining_processes >= MAX_PROCESSES) {
        ker_write("Max processes reached\n");
        return -1;
    }

    process_t *process = (process_t *) mem_alloc(sizeof(process_t));
    if (process == NULL) {
        ker_write("Error creating process\n");
        return -1;
    }

     init_process(process, scheduler->next_unused_pid, code, args, argc, name, priority, unkilliable); 
   
    node_t *process_node = mem_alloc(sizeof(node_t));
    if (process_node == NULL) {
        ker_write("Error creating process node\n");
        mem_free(process);  // Liberar proceso creado en caso de error
        return -1;
    } 

    process_node->process = (void *) process;
    if (process->pid != IDLE_PID) {
        for (int i = 0; i < process->priority; i++) {
            addNode(scheduler->process_list, (void *) process);
         }

    }

    scheduler->processes[process->pid] = process_node;
    scheduler->next_unused_pid = (scheduler->next_unused_pid + 1) % MAX_PROCESSES;
    scheduler->remaining_processes++;

    return process->pid;
}


int32_t waitpid(int32_t pid, int *status) {
    scheduler_adt scheduler = getSchedulerADT();
    int32_t current_pid = get_current_pid();
    process_t *current_process = (process_t *)scheduler->processes[current_pid]->process;
    
    if (pid == -1) {
        int has_children = 0;
        
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (scheduler->processes[i] != NULL) {
                process_t *proc = (process_t *)scheduler->processes[i]->process;
                if (proc->parent_pid == current_pid) {
                    has_children = 1;
                    
                    if (proc->state == KILLED && !proc->has_been_waited) {
                        proc->has_been_waited = 1;
                        if (status != NULL) {
                            *status = proc->exit_code;
                        }
                        return proc->pid;
                    }
                }
            }
        }
        
        if (!has_children) {
            return -1;
        }

        // current_process->state = WAITING_FOR_CHILD;
        removeAllNodes(scheduler->process_list, current_process);
        block_process(current_pid);
        
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (scheduler->processes[i] != NULL) {
                process_t *proc = (process_t *)scheduler->processes[i]->process;
                if (proc->parent_pid == current_pid && 
                    proc->state == KILLED && 
                    !proc->has_been_waited) {
                    proc->has_been_waited = 1;
                    if (status != NULL) {
                        *status = proc->exit_code;
                    }
                    return proc->pid;
                }
            }
        }
    } else {
        if (pid < 0 || pid >= MAX_PROCESSES || scheduler->processes[pid] == NULL) {
            return -1;
        }
        
        process_t *target = (process_t *)scheduler->processes[pid]->process;
        
        if (target->parent_pid != current_pid) {
            return -1;
        }
        
        if (target->state == KILLED && !target->has_been_waited) {
            target->has_been_waited = 1;
            if (status != NULL) {
                *status = target->exit_code;
            }
            return pid;
        }
        
        // current_process->state = WAITING_FOR_CHILD;
        removeAllNodes(scheduler->process_list, current_process);
        block_process(current_pid);
        
        if (target->state == KILLED && !target->has_been_waited) {
            target->has_been_waited = 1;
            if (status != NULL) {
                *status = target->exit_code;
            }
            return pid;
        }
    }
    
    return -1;
}

process_t * get_current_process() {
    scheduler_adt scheduler = getSchedulerADT();
    return (process_t *) scheduler->processes[scheduler->current_pid]->process;
}

void process_priority(uint64_t pid, uint8_t new_prio) {
    scheduler_adt scheduler = getSchedulerADT();

    new_prio = CAPPED_PRIORITY(new_prio);  

    process_t *current_process = (process_t *) scheduler->processes[pid]->process;
    int priority_delta = new_prio - current_process->priority;
    
    // Case when we have to add to unblock 
    if (priority_delta == 0) {
        for (int i = 0; i < current_process->priority; i++) {
            //ker_write("UNBLOCKING\n");
            if (current_process->state!=READY && current_process->state!=RUNNING)
            addNode(scheduler->process_list, (void *) current_process);
        }
    }

    if (priority_delta > 0) {
        for (int i = 0; i < priority_delta; i++) {
            addNode(scheduler->process_list, (void *) current_process);
        }
    } else {
        for (int i = 0; i < abs(priority_delta); i++) {
            removeNode(scheduler->process_list, (void *) current_process);
        }
    }

    current_process->priority = new_prio;  
}


// Terminar un proceso
int kill_process(uint16_t pid) {
    scheduler_adt scheduler = getSchedulerADT();
    if (pid == IDLE_PID) {
        ker_write("Cannot kill idle process\n");
        return -1;
    }

    if (scheduler->processes[pid] == NULL) {
        ker_write("Process not found\n");
        return -1;
    }

    process_t *process_to_kill = (process_t *)scheduler->processes[pid]->process;
    if (process_to_kill->unkilliable) {
        ker_write("Failed to kill process. Process is unkilliable\n");
        return -1;
    }

    if (process_to_kill->state == BLOCKED) {
        removeAllNodes(scheduler->blocked_process_list, (void *)process_to_kill);
    } else {
        removeAllNodes(scheduler->process_list, (void *)process_to_kill);
    }

    process_to_kill->state = KILLED;

    // if (process_to_kill->parent_pid != -1) {
    //     process_t *parent = (process_t *)scheduler->processes[process_to_kill->parent_pid]->process;
    //     if (parent->state == WAITING_FOR_CHILD) {
    //         unblock_process(parent->pid);
    //     }
    // }

    yield();
    return 0;
}

void kill_current_process() {
    scheduler_adt scheduler = getSchedulerADT();
    kill_process(scheduler->current_pid);
}

void yield() {
    scheduler_adt scheduler = getSchedulerADT();
    scheduler->current_quantum = 0;
    asm_do_timer_tick();
}

// Bloquear un proceso
int block_process(uint64_t pid) {
    scheduler_adt scheduler = getSchedulerADT();
    if (scheduler->processes[pid] == NULL || pid == IDLE_PID) {
        ker_write("Process %llu not found\n", pid);
        return -1;
    }
    //ker_write("Blocking process\n");
    process_t *process_to_block = (process_t *) scheduler->processes[pid]->process;
    process_to_block->state = BLOCKED;
    // ker_write("pid blocked");
    // print_number2(pid);
    // ker_write("\n");
    removeAllNodes(scheduler->blocked_process_list, process_to_block);
    addNode(scheduler->blocked_process_list, process_to_block);
    removeAllNodes(scheduler->process_list, process_to_block);
    yield();
    
    return 0;
}

// Desbloquear un proceso
int unblock_process(uint64_t pid) {
    scheduler_adt scheduler = getSchedulerADT();
    if (scheduler->processes[pid] == NULL) {
        return -1;
    }

    process_t *process_to_unblock = (process_t *) scheduler->processes[pid]->process;
    process_to_unblock->state = READY;
    // ker_write("pid unblocked: ");
    // print_number2(pid);
    // ker_write("\n");
    removeAllNodes(scheduler->blocked_process_list, process_to_unblock);
    for (int i =0 ; i < process_to_unblock->priority;i++){
        addNode(scheduler->process_list,process_to_unblock);
    }
   // process_priority(pid, process_to_unblock->priority);
    yield();

    return 0;
}

uint32_t get_current_pid() {
    scheduler_adt scheduler = getSchedulerADT();
    return scheduler->current_pid;
}

process_t *get_process_by_pid(uint32_t pid) {
    scheduler_adt scheduler = getSchedulerADT();
    return (process_t *) scheduler->processes[pid]->process;
}
#undef CAPPED_PRIORITY


void int_to_string2(int num, char *str) {
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

void print_number2(int number) {
    char buffer[20]; // Buffer to hold the string representation of the number
    int_to_string2(number, buffer); // Convert integer to string
    ker_write(buffer); // Print the number
}


