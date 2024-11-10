#include "../include/scheduler.h"
#include "../include/memman.h"
#include "../include/linkedListADT.h"
#include <io.h>
#include <stdlib.h>
#include <process.h>
#include <math.h>
#include <IdtLoader.h>
#include <lib.h>
#include <semaphore.h>

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

uint8_t sig_fg_kill = 0;

void print_process_lists();

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

    sig_fg_kill = 0;

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
    return IDLE_PID; 

}


void* scheduler(void* stack_pointer) {
    scheduler_adt scheduler = getSchedulerADT();

    process_t *current_process = NULL;
    process_t *next_process = NULL;

    if (scheduler->current_pid != -1) {
        current_process = (process_t *)scheduler->processes[scheduler->current_pid]->process;
        current_process->stack_pointer = stack_pointer;
    }

    scheduler->current_quantum--;
    
   
    if (scheduler->current_quantum > 0 && 
        current_process != NULL && 
        (current_process->state == RUNNING || current_process->state == READY)) {
        return stack_pointer;
    }

   
    int32_t next_pid = get_next_ready_pid();
    
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
    

    scheduler->current_pid = next_pid;
    next_process = (process_t *)scheduler->processes[next_pid]->process;
    next_process->state = RUNNING;
    
    scheduler->current_quantum = DEFAULT_QUANTUM;

   
   
    return next_process->stack_pointer;
}

static void add_children(process_t* parent, process_t* child) {
    if (parent == NULL || child == NULL) {
        return;
    }
    addNode(parent->children, (void *)child);
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
    
    
    if (priority_delta == 0) {
        for (int i = 0; i < current_process->priority; i++) {
           
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

static void remove_children(linkedListADT children, process_t *child) {
    if (children == NULL || child == NULL) {
        return;
    }

    removeNode(children, (void *)child);
}

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

    remove_from_all_semaphores(pid);


    if (process_to_kill->state == BLOCKED) {
        removeAllNodes(scheduler->blocked_process_list, (void *)process_to_kill);
    } else {
        removeAllNodes(scheduler->process_list, (void *)process_to_kill);
    }

    if(process_to_kill->parent_pid != IDLE_PID){
        process_t *parent = get_process_by_pid(process_to_kill->parent_pid);
        remove_children(parent->children, process_to_kill);
    }
    
        for (int i = 0; i < MAX_PROCESSES; i++) {
        if (scheduler->processes[i] != NULL) {
            process_t *process = (process_t *)scheduler->processes[i]->process;
            if (process->parent_pid == pid) {
                kill_process(process->pid);
            }
        }
    }

    scheduler->processes[pid]->process = NULL;
    scheduler->next_unused_pid = pid;
    


    free_process(process_to_kill);
    scheduler->remaining_processes--;
    process_to_kill->state = KILLED;

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

    process_t *process_to_block = (process_t *) scheduler->processes[pid]->process;
    process_to_block->state = BLOCKED;
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
    removeAllNodes(scheduler->blocked_process_list, process_to_unblock);
    for (int i =0 ; i < process_to_unblock->priority;i++){
        addNode(scheduler->process_list,process_to_unblock);
    }
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
    if (process == NULL){
        return -1;
    }
    process->fds[fd_index] = new_fd;
    return 0;
}

void kill_current_ppid() {
    process_t *process = get_current_process();
    if (process->parent_pid != IDLE_PID) {
        kill_process(process->parent_pid);
    } else {
        kill_current_process();
    }
}

void kill_fg_process() {
    process_t* current = get_current_process();
    
    if (current == NULL || current->pid == IDLE_PID) {
        return;
    }
    
    if (current->fds[STDIN] == STDIN) {
        ker_write("Killing current process\n");
        if (current->parent_pid != IDLE_PID) {
            kill_process(current->parent_pid);
            print_number(current->parent_pid);
        } else {
            print_number(current->pid);
            kill_process(current->pid);
        }
    }
}

void print_process_lists() {
    scheduler_adt scheduler = getSchedulerADT();
    
    // Print processes in the process list
    node_t *process_node = getFirstNode(scheduler->process_list);
    ker_write("Processes in the process list:\n");
    while (process_node != NULL) {
        process_t *process = (process_t *)process_node->process;
        ker_write("pid: ");
        print_number(process->pid);
        ker_write(" parent_pid: ");
        print_number(process->parent_pid);
        ker_write(" state: ");
        print_number(process->state);
        ker_write("\n");
        process_node = (node_t *) (process_node->next);
    }

    // Print blocked processes
    node_t *blocked_node = getFirstNode(scheduler->blocked_process_list);
    ker_write("Blocked processes:\n");
    while (blocked_node != NULL) {
        process_t *blocked_process = (process_t *)blocked_node->process;
        print_number(blocked_process->pid);
        blocked_node = (node_t *) (process_node->next);
    }
}

#undef CAPPED_PRIORITY
