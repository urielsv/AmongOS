#include "../include/scheduler.h"
#include "../include/memman.h"
#include "../include/linkedListADT.h"
#include <io.h>
#include <stdlib.h>
#include <process.h>
#include <math.h>
#include <IdtLoader.h>

#define IDLE_PID 0
#define DEFAULT_QUANTUM 5

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
    scheduler_adt scheduler = getSchedulerADT();
    
    // Disable interrupts during scheduling
    asm_cli();

    process_t *current_process = NULL;
    process_t *next_process = NULL;

    // Save current context if there is a current process
    if (scheduler->current_pid != -1) {
        current_process = (process_t *)scheduler->processes[scheduler->current_pid]->process;
        current_process->stack_pointer = stack_pointer;
    }

    scheduler->current_quantum--;
    
    // Don't switch if quantum hasn't expired and process isn't blocked/killed
    if (scheduler->current_quantum > 0 && 
        current_process != NULL && 
        (current_process->state == RUNNING || current_process->state == READY)) {
        asm_sti();
        return stack_pointer;
    }

    // Get next process to run
    int32_t next_pid = get_next_ready_pid();
    
    // Handle state transition atomically
    if (current_process != NULL) {
        switch (current_process->state) {
            case RUNNING:
                current_process->state = READY;
                swapToLast(scheduler->process_list, current_process);
                break;
            case KILLED:
                scheduler->processes[scheduler->current_pid] = NULL;
                free_process(current_process);
                scheduler->remaining_processes--;
                break;
            case BLOCKED:
                // Already handled by block_process()
                break;
            default:
                break;
        }
    }

    // Set up next process
    scheduler->current_pid = next_pid;
    next_process = (process_t *)scheduler->processes[next_pid]->process;
    next_process->state = RUNNING;
    
    scheduler->current_quantum = DEFAULT_QUANTUM;
    
    // Re-enable interrupts before returning
    asm_sti();
    
    return next_process->stack_pointer;
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
    
    // Helper function to check if a process is a finished child
    int check_finished_child(process_t *proc, int32_t parent_pid) {
        return (proc->parent_pid == parent_pid && 
                proc->state == KILLED && 
                !proc->has_been_waited);
    }
    
    int32_t handle_finished_process(process_t *proc, int *status) {
        proc->has_been_waited = 1;
        if (status != NULL) {
            *status = proc->exit_code;
        }
        return proc->pid;
    }
    
    if (pid == -1) {
        // Check for any children first
        int has_children = 0;
        
        // First pass: look for finished children
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (scheduler->processes[i] != NULL) {
                process_t *proc = (process_t *)scheduler->processes[i]->process;
                if (proc->parent_pid == current_pid) {
                    has_children = 1;
                    if (check_finished_child(proc, current_pid)) {
                        return handle_finished_process(proc, status);
                    }
                }
            }
        }
        
        if (!has_children) {
            return -1;  
        }
    } else {
        // Wait for specific process
        if (pid < 0 || pid >= MAX_PROCESSES || scheduler->processes[pid] == NULL) {
            return -1;
        }
        
        process_t *target = (process_t *)scheduler->processes[pid]->process;
        if (target->parent_pid != current_pid) {
            return -1;  // Not a child of current process
        }
        
        if (check_finished_child(target, current_pid)) {
            return handle_finished_process(target, status);
        }
    }
    
    // If we get here, we need to block and wait
    removeAllNodes(scheduler->process_list, current_process);
    block_process(current_pid);
    
    return -1;  // Will never actually return this as process is blocked
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
        return -1;
    }

   // Disable interrupts during critical section
    asm_cli();

    process_t *process_to_block = (process_t *) scheduler->processes[pid]->process;
    process_to_block->state = BLOCKED;
    removeAllNodes(scheduler->blocked_process_list, process_to_block);
    addNode(scheduler->blocked_process_list, process_to_block);
    removeAllNodes(scheduler->process_list, process_to_block);
    
    // Re-enable interrupts 
    asm_sti();
    
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


