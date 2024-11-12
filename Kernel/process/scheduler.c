#include "../include/scheduler.h"
#include "../include/linked_list_adt.h"
#include <io.h>
#include <stdlib.h>
#include <process.h>
#include <math.h>
#include <idt_loader.h>
#include <lib.h>
#include <semaphore.h>
#include <buddy_memman.h>

#define idle_pid 0
#define default_quantum 5

#define capped_priority(prio) (prio >= MAX_PRIORITY ? MAX_PRIORITY : prio)

typedef struct scheduler_cdt {
    node_t * processes[MAX_PROCESSES];
    linked_list_adt blocked_process_list;
    linked_list_adt process_list;
    int32_t current_pid;
    uint16_t next_unused_pid;
    uint16_t remaining_processes;
    int32_t current_quantum;
} scheduler_cdt;

uint8_t sig_fg_kill = 0;

void print_process_lists();

scheduler_adt init_scheduler() {
    scheduler_adt scheduler = (scheduler_adt)SCHEDULER_ADDRESS;

    scheduler->process_list = create_linked_list();
    scheduler->blocked_process_list = create_linked_list();

    for (int i = 0; i <MAX_PROCESSES; i++) {
        scheduler->processes[i] = NULL;
    }

    scheduler->remaining_processes=0;
    scheduler->next_unused_pid = 0;
    scheduler->current_pid = -1;
    scheduler->current_quantum = default_quantum;

    sig_fg_kill = 0;

    return scheduler;
}

scheduler_adt get_scheduler_adt() {
    return (scheduler_adt)SCHEDULER_ADDRESS;
}


int32_t get_next_ready_pid() {
    
    scheduler_adt scheduler = get_scheduler_adt();
    start_iterator(scheduler->process_list);
    while(has_next(scheduler->process_list)){
        process_t * process = (process_t *) get_next(scheduler->process_list);
        return process->pid;
    }
    
    return idle_pid; 
}


void* scheduler(void* stack_pointer) {
    scheduler_adt scheduler = get_scheduler_adt();

    process_t *current_process = NULL;
    process_t *next_process = NULL;

    if (scheduler->current_pid != -1) {
        current_process = (process_t *)scheduler->processes[scheduler->current_pid]->process;
        current_process->stack_pointer = stack_pointer;
    }

    scheduler->current_quantum--;
    
   
    if (scheduler->current_quantum > 0 && 
        current_process != NULL && 
        (current_process->state == running || current_process->state == ready)) {
        return stack_pointer;
    }

   
    
        switch (current_process->state) {
            case running:
                current_process->state = ready;
                swap_to_last(scheduler->process_list, current_process);
                break;
            case killed:
                
                break;
            case blocked: 
                break;
            case waiting_for_child:
                start_iterator(current_process->children);
                while(has_next(current_process->children)){
                    process_t *child = (process_t *) get_next(current_process->children);
                    if(child == NULL || child->parent_pid != current_process->pid){
                        remove_node(current_process->children, child);
                        start_iterator(current_process->children);
                    }
                }
                if(is_empty_list(current_process->children)){
                    current_process->state = ready;
                }
                for(int i = 0; i < current_process->priority; i++){
                    swap_to_last(scheduler->process_list, current_process);
                }
                break;
            default:
                break;
        }
    
    int32_t next_pid = get_next_ready_pid();

    scheduler->current_pid = next_pid;
    next_process = (process_t *)scheduler->processes[next_pid]->process;
    next_process->state = running;
    
    scheduler->current_quantum = default_quantum;

   
   
    return next_process->stack_pointer;
}

static uint32_t get_next_unused_pid(){
    scheduler_adt scheduler = get_scheduler_adt();
    int i = idle_pid + 1;
    while(scheduler->processes[i] != NULL){
        i++;
    }
    return i;
}


int32_t create_process(function code, char **args, int argc, char *name, uint8_t priority, uint8_t unkilliable) {

    priority = capped_priority(priority);
    scheduler_adt scheduler = get_scheduler_adt();
    if (scheduler->remaining_processes >=MAX_PROCESSES) {
        ker_write("max processes reached\n");
        return -1;
    }

    process_t *process = (process_t *) b_alloc(sizeof(process_t));
    if (process == NULL) {
        ker_write("error creating process\n");
        return -1;
    }
    
    init_process(process, scheduler->next_unused_pid, code, args, argc, name, priority, unkilliable); 


   
    node_t *process_node = b_alloc(sizeof(node_t));
    if (process_node == NULL) {
        ker_write("error creating process node\n");
        b_free(process);  
        return -1;
    } 

    process_node->process = (void *) process;
    if (process->pid != idle_pid) {
        for (int i = 0; i < process->priority; i++) {
            add_node(scheduler->process_list, (void *) process);
         }

    }

    scheduler->processes[process->pid] = process_node;
    scheduler->next_unused_pid = get_next_unused_pid();
    scheduler->remaining_processes++;

    return process->pid;
}


void waitpid(uint32_t child_pid){

    process_t * parent = get_current_process();
    process_t * child = get_process_by_pid(child_pid);
    if(child == NULL || child->parent_pid != parent->pid){
        return;
    }

    add_node(parent->children ,(void *) child);
    parent->state = waiting_for_child;
    yield();
}


process_t * get_current_process() {
    scheduler_adt scheduler = get_scheduler_adt();
    return (process_t *) scheduler->processes[scheduler->current_pid]->process;
}

void process_priority(uint64_t pid, uint8_t new_prio) {
    scheduler_adt scheduler = get_scheduler_adt();

    new_prio = capped_priority(new_prio);  

    process_t *current_process = (process_t *) scheduler->processes[pid]->process;
    int priority_delta = new_prio - current_process->priority;
    
    
    if (priority_delta == 0) {
        for (int i = 0; i < current_process->priority; i++) {
           
            if (current_process->state!=ready && current_process->state!=running)
            add_node(scheduler->process_list, (void *) current_process);
        }
    }

    if (priority_delta > 0) {
        for (int i = 0; i < priority_delta; i++) {
            add_node(scheduler->process_list, (void *) current_process);
        }
    } else {
        for (int i = 0; i < abs(priority_delta); i++) {
            remove_node(scheduler->process_list, (void *) current_process);
        }
    }

    current_process->priority = new_prio;  
}



uint8_t last_child(uint32_t parent_pid) {
    scheduler_adt scheduler = get_scheduler_adt();
    process_t *parent = (process_t *)scheduler->processes[parent_pid]->process;
    if (parent == NULL) {
        return 0;
    }

    return is_empty_list(parent->children);
}

int kill_process(uint32_t pid) {
    scheduler_adt scheduler = get_scheduler_adt();
    if (pid == idle_pid) {
        ker_write("\n_cannot kill idle process\n");
        return -1;
    }

    if (scheduler->processes[pid] == NULL) {
        ker_write("process not found\n");
        return -1;
    }

    process_t *process_to_kill = (process_t *)scheduler->processes[pid]->process;
    if (process_to_kill->unkilliable) {
        ker_write("failed to kill process. process is unkilliable\n");
        return -1;
    }

    remove_from_all_semaphores(pid);


    if (process_to_kill->state == blocked) {
        remove_all_nodes(scheduler->blocked_process_list, (void *)process_to_kill);
    } else {
        remove_all_nodes(scheduler->process_list, (void *)process_to_kill);
    }

    if(process_to_kill->parent_pid != idle_pid){
        
    }
    
        for (int i = 0; i <MAX_PROCESSES; i++) {
        if (scheduler->processes[i] != NULL) {
            process_t *process = (process_t *)scheduler->processes[i]->process;
            if (process->parent_pid == pid) {
                block_process(process->pid);
                process->parent_pid = idle_pid;
            }
        }
    }

    scheduler->next_unused_pid = pid;
    
    free_process(process_to_kill);
    scheduler->remaining_processes--;
    process_to_kill->state = killed;
    
    yield();
    return 0;
}

void kill_current_process() {
    scheduler_adt scheduler = get_scheduler_adt();
    kill_process(scheduler->current_pid);
}

void yield() {
    scheduler_adt scheduler = get_scheduler_adt();
    scheduler->current_quantum = 0;
    asm_do_timer_tick();
}

int block_process(uint64_t pid) {
    scheduler_adt scheduler = get_scheduler_adt();
    if (scheduler->processes[pid] == NULL || pid == idle_pid) {
        return -1;
    }

    process_t *process_to_block = (process_t *) scheduler->processes[pid]->process;
    process_to_block->state = blocked;
    remove_all_nodes(scheduler->blocked_process_list, process_to_block);
    add_node(scheduler->blocked_process_list, process_to_block);
    remove_all_nodes(scheduler->process_list, process_to_block);
     
    return 0;
}


int unblock_process(uint64_t pid) {
    scheduler_adt scheduler = get_scheduler_adt();
    if (scheduler->processes[pid] == NULL) {
        return -1;
    }

    process_t *process_to_unblock = (process_t *) scheduler->processes[pid]->process;
    process_to_unblock->state = ready;
    remove_all_nodes(scheduler->blocked_process_list, process_to_unblock);
    for (int i =0 ; i < process_to_unblock->priority;i++){
        add_node(scheduler->process_list,process_to_unblock);
    }
    return 0;
}

uint32_t get_current_pid() {
    scheduler_adt scheduler = get_scheduler_adt();
    return scheduler->current_pid;
}

process_t *get_process_by_pid(uint32_t pid) {
    scheduler_adt scheduler = get_scheduler_adt();
    if (scheduler->processes[pid] == NULL) {
        return NULL;
    }

    return (process_t *) scheduler->processes[pid]->process;
}

int16_t get_current_process_file_descriptor(uint8_t fd_index) {
	scheduler_adt scheduler = get_scheduler_adt();
	process_t *process = scheduler->processes[scheduler->current_pid]->process;
	return process->fds[fd_index];
}

uint16_t change_process_fd(uint32_t pid, uint16_t fd_index, int16_t new_fd){
    scheduler_adt scheduler = get_scheduler_adt();
    process_t* process = (process_t *) scheduler->processes[pid]->process;
    if (process == NULL){
        return -1;
    }
    process->fds[fd_index] = new_fd;
    return 0;
}

void kill_current_ppid() {
    process_t *process = get_current_process();
    if (process->parent_pid != idle_pid) {
        kill_process(process->parent_pid);
    } else {
        kill_current_process();
    }
}

void kill_fg_process() {
    process_t* current = get_current_process();
    
    if (current == NULL || current->pid == idle_pid) {
        return;
    }
    
    if (current->fds[STDIN] == STDIN) {
        ker_write("killing current process\n");
        if (current->parent_pid != idle_pid) {
            kill_process(current->parent_pid);
            print_number(current->parent_pid);
        } else {
            print_number(current->pid);
            kill_process(current->pid);
        }
    }
}

void print_process_lists() {
    scheduler_adt scheduler = get_scheduler_adt();
    
    node_t *process_node = get_first_node(scheduler->process_list);
    ker_write("processes in the process list:\n");
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

    node_t *blocked_node = get_first_node(scheduler->blocked_process_list);
    ker_write("blocked processes:\n");
    while (blocked_node != NULL) {
        process_t *blocked_process = (process_t *)blocked_node->process;
        print_number(blocked_process->pid);
        blocked_node = (node_t *) (process_node->next);
    }
}

#undef capped_priority
