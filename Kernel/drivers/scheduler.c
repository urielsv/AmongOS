#include "../include/scheduler.h"
#include "../include/memman.h"
#include "../include/process.h"
#include "../include/linkedListADT.h"
#include <io.h>
#include <stdlib.h>

#define IDLE_PID 0
#define DEFAULT_QUANTUM 2

typedef struct scheduler_cdt {
    node_t * processes[MAX_PROCESSES];
    linkedListADT blocked_process_list;
    linkedListADT process_list;
    int32_t current_pid;
    uint16_t next_unused_pid;
    uint16_t remaining_processes;
    int32_t current_quantum;
} scheduler_cdt;

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
        ker_write("!!!");
        return ((process_t *) next_node->process)->pid;
    }
    return IDLE_PID;  // Si no hay otro proceso listo, regresar al proceso idle


    // for (int i = 0; i < MAX_PROCESSES; i++) {
    //     scheduler->current_pid = (scheduler->current_pid + 1) % MAX_PROCESSES;
    //     if (scheduler->processes[scheduler->current_pid] != NULL && 
    //         ((process_t *)scheduler->processes[scheduler->current_pid]->process)->state == READY) {
    //         return scheduler->current_pid;
    //     }
    // }
}


void* scheduler(void* stack_pointer) {
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

 //ker_write("enter switch cases\n");
 
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

            case BLOCKED:
            default:
               // ker_write("Other state");
                break;
        }
        scheduler->current_quantum = DEFAULT_QUANTUM;
        return current_process->stack_pointer;    
}



// Crear un nuevo proceso
int32_t create_process(Function code, char **args, int argc, char *name, uint8_t priority, uint8_t unkillable) {
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

     init_process(process, scheduler->next_unused_pid, code, args, argc, name, priority, unkillable); 
   
    node_t *process_node = mem_alloc(sizeof(node_t));
    if (process_node == NULL) {
        ker_write("Error creating process node\n");
        mem_free(process);  // Liberar proceso creado en caso de error
        return -1;
    } 

    process_node->process = (void *) process;
    if (process->pid != IDLE_PID) {
        // for (int i = 0; i < process->priority; i++) {
            addNode(scheduler->process_list, (void *) process);
        // }
    }

    scheduler->processes[process->pid] = process_node;
    scheduler->next_unused_pid = (scheduler->next_unused_pid + 1) % MAX_PROCESSES;
    scheduler->remaining_processes++;

    return process->pid;
}

// Terminar un proceso
int kill_process(uint16_t pid) {
    scheduler_adt scheduler = getSchedulerADT();
    if (pid == IDLE_PID) {
        ker_write("Cannot kill idle process\n");
        return -1;
    }

    if (scheduler->processes[pid] == NULL) {
        ker_write("Process %llu not found\n", pid);
        return -1;
    }

    process_t *process_to_kill = (process_t *) scheduler->processes[pid]->process;
    if (process_to_kill->unkilliable) {
        return -1;
    }

    if (process_to_kill->state == BLOCKED) {
        removeNode(scheduler->blocked_process_list, process_to_kill);
    } else {
        for (int i = 0; i < process_to_kill->priority; i++) {
            removeNode(scheduler->process_list, process_to_kill);
        }
    }

    scheduler->processes[pid] = NULL;
    scheduler->remaining_processes--;
    return 0;
}

// Bloquear un proceso
int block_process(uint64_t pid) {
    scheduler_adt scheduler = getSchedulerADT();
    if (scheduler->processes[pid] == NULL || pid == IDLE_PID) {
        ker_write("Process %llu not found\n", pid);
        return -1;
    }

    process_t *process_to_block = (process_t *) scheduler->processes[pid]->process;
    process_to_block->state = BLOCKED;
    addNode(scheduler->blocked_process_list, process_to_block);
    for (int i = 0; i < process_to_block->priority; i++) {
        removeNode(scheduler->process_list, process_to_block);
    }

    return 0;
}

// Desbloquear un proceso
int unblock_process(uint64_t pid) {
    scheduler_adt scheduler = getSchedulerADT();
    if (scheduler->processes[pid] == NULL) {
        ker_write("Process %llu not found\n", pid);
        return -1;
    }

    process_t *process_to_unblock = (process_t *) scheduler->processes[pid]->process;
    process_to_unblock->state = READY;
    removeNode(scheduler->blocked_process_list, process_to_unblock);
    for (int i = 0; i < process_to_unblock->priority; i++) {
        addNode(scheduler->process_list, process_to_unblock);
    }

    return 0;
}
