#include "../include/scheduler.h"
#include "../include/memman.h"
#include "../include/linkedListADT.h"
#include <io.h>
#include <stdlib.h>
#include <process.h>
#include <math.h>
#include <IdtLoader.h>
#include <lib.h>

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
   //ker_write("cs");

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
        return stack_pointer;
    }

    // Get next process to run
    int32_t next_pid = get_next_ready_pid();
    

    // Handle state transition atomically
        switch (current_process->state) {
            case RUNNING:
                current_process->state = READY;
                swapToLast(scheduler->process_list, current_process);
                break;
            case KILLED:
                
                break;
            case BLOCKED: 
                break;
            case WAITING_FOR_CHILD:
                start_iterator(current_process->children);
                while(has_next(current_process->children)){
                    process_t *child = (process_t *) get_next(current_process->children);
                    if(child == NULL || child->parent_pid != current_process->pid){
                        removeNode(current_process->children, child);
                        start_iterator(current_process->children);
                    }
                }
                if(isEmptyList(current_process->children)){
                    current_process->state = READY;
                }
            default:
                break;
        }

    // Set up next process
    //print_number2(scheduler->current_pid);
    scheduler->current_pid = next_pid;
    //print_number2(next_pid);
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


void waitpid(uint32_t child_pid){

    process_t * parent = get_current_process();
    process_t * child = get_process_by_pid(child_pid);
    if(child == NULL || child->parent_pid != parent->pid){
        return;
    }

    addNode(parent->children ,(void *) child);
    parent->state = WAITING_FOR_CHILD;
    yield();
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
int kill_process(uint32_t pid) {
    scheduler_adt scheduler = getSchedulerADT();
    if (pid == IDLE_PID) {
        ker_write("\nCannot kill idle process\n");
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

    scheduler->processes[pid]->process = NULL;
    scheduler->next_unused_pid = pid;
    free_process(process_to_kill);
    scheduler->remaining_processes--;

    // al dope
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
    if (scheduler->processes[pid] == NULL) {
        return NULL;
    }

    return (process_t *) scheduler->processes[pid]->process;
}

int16_t get_current_process_file_descriptor(uint8_t fd_index) {
	scheduler_adt scheduler = getSchedulerADT();
	process_t *process = scheduler->processes[scheduler->current_pid]->process;
	return process->fds[fd_index];
}

//Simulating dup2().
uint16_t change_process_fd(uint32_t pid, uint16_t fd_index, int16_t new_fd){
    scheduler_adt scheduler = getSchedulerADT();
    process_t* process = (process_t *) scheduler->processes[pid]->process;
    ker_write("changing fd\n");
    if (process == NULL){
        return -1;
    }
    process->fds[fd_index] = new_fd;
    print_number(process->fds[fd_index]);
    return 0;
}

#undef CAPPED_PRIORITY
