#include "../include/scheduler.h"
#include "../include/memman.h"
#include "../include/process.h"
#define IDLE_PID 0

// typedef struct {

//     uint64_t pid;
//     uint16_t priority;
//     uint16_t state;
//     void * stack_base;
//     void * stack_pointer;
//     char ** argv;
//     uint64_t argc;

// } process_t;

typedef struct process_node {

    process_t* process;
    struct process_node* next;

} process_node_t;

struct scheduler_cdt {
    int process_count;
    process_node_t* head;
    process_node_t* current;
    process_t* processes[MAX_PROCESSES];
    uint16_t current_pid;
};


//enum priority { LOW=0, LOW_MEDIUM, MEDIUM, HIGH_MEDIUM, HIGH };


scheduler_adt init_scheduler() {

    scheduler_adt scheduler = (scheduler_adt) SCHEDULER_ADDRESS;

    scheduler->process_count = 0;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        scheduler->processes[i] = NULL;
    }

    scheduler->head = NULL;
    scheduler->current = NULL;

    scheduler->current_pid = 0;
    return scheduler;
}

scheduler_adt getSchedulerADT() {
	return (scheduler_adt) SCHEDULER_ADDRESS;
}

//Agrega o remueve nodos de la lista de punteros a procesos segun la prioridad.
int32_t set_priority(uint16_t pid, uint8_t new_priority) {

    scheduler_adt scheduler = getSchedulerADT();
    process_t * process_pointer = scheduler->processes[pid];

    if (process_pointer == NULL || pid == IDLE_PID || new_priority > HIGH)
		return -1;

    int old_priority = process_pointer->priority;
    process_pointer->priority = new_priority;

    //Es una manera ineficiente de hacerlo, pero la idea es que sea funcional.
    remove_all_process_appearances_from_list(pid);
    for (int i = 0; i <= new_priority; i++) {
        add_process_to_list(process_pointer);
    }

    return new_priority;

}


void* scheduler(void* stack_pointer) {

    scheduler_adt scheduler = getSchedulerADT();

    if (scheduler->head == NULL) {
        return stack_pointer;
    }

    if (scheduler->current != NULL) {
        scheduler->current->process->stack_pointer = stack_pointer;
    }

    if (scheduler->current == NULL || scheduler->current->next == NULL) {
        scheduler->current = scheduler->head;
    } else {
        scheduler->current = scheduler->current->next;
    }

    return scheduler->current->process->stack_pointer;
}

int add_process_to_list(process_t* process) {

    scheduler_adt scheduler = getSchedulerADT();

    process_node_t* new_node = mem_alloc(sizeof(process_node_t));
    if (new_node == NULL) {
        return -1;
    }

    new_node->process = process;
    new_node->next = NULL;

    if (scheduler->head == NULL) {
        scheduler->head = new_node;
    } else {
        process_node_t* current = scheduler->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }

    return 0;
}


int create_process(Function code, int argc, char **argv) {

    scheduler_adt scheduler = getSchedulerADT();

    if (scheduler->process_count >= MAX_PROCESSES) {
        printf("Max processes reached\n");
        return -1;
    }

    process_t *process = (process_t *) mem_alloc(sizeof(process_t));

    if (process == NULL) {
        printf("Error creating process\n");
        return -1;
    }

   process->stack_base = mem_alloc(STACK_SIZE);

    if (process->stack_base == NULL) {
        printf("Error creating stack frame\n");
        return -1;
    }

    process->argv = argv;
    process->argc = argc;
    //process->pid = pid(); cual es el pid que deberia ir?
    //process->state = READY;
    process->priority = LOW;
    process->stack_pointer = create_process_stack_frame ((void *) code, (void *) ((uint64_t) process->stack_base + STACK_SIZE), (void *) argv);

    if (add_process_to_list(process) == -1) {
        printf("Error adding process\n");
        return -1;
    }

    scheduler->processes[process->pid] = process;
    scheduler->process_count++;

    return process->pid;
}


void remove_all_process_appearances_from_list(uint64_t pid) {

    scheduler_adt scheduler = getSchedulerADT();

    //remove process from the process pointer list
    process_node_t* current = scheduler->head;
    process_node_t* prev = NULL;

    while (current != NULL) {
        if (current->process->pid == pid) {
            if (prev == NULL) {
                scheduler->head = current->next;
            } else {
                prev->next = current->next;
            }

            if (current->process == scheduler->current->process) {
                scheduler->current = scheduler->current->next;
                if (scheduler->current == NULL) {
                    scheduler->current = scheduler->head;
                }
            }

            mem_free(current);
            scheduler->process_count--;
            return;
        }
        prev = current;
        current = current->next;
    }
}

int kill_process(uint64_t pid) {

    scheduler_adt scheduler = getSchedulerADT();

    if (scheduler->processes[pid] == NULL) {
        printf("Process %llu not found\n", pid);
        return -1;
    }

    remove_all_process_appearances_from_list(pid);
    mem_free(scheduler->processes[pid]->stack_base);
    mem_free(scheduler->processes[pid]->stack_pointer);
    mem_free(scheduler->processes[pid]->argv);
    mem_free(scheduler->processes[pid]);
    scheduler->processes[pid] = NULL;

    return 0;
}

int block_process(uint64_t pid) {

    scheduler_adt scheduler = getSchedulerADT();

    if (scheduler->processes[pid] == NULL) {
        printf("Process %llu not found\n", pid);
        return -1;
    }

    //scheduler->processes[pid]->state = BLOCKED;
    remove_all_process_appearances_from_list(pid);
    return 0;
}

int unblock_process(uint64_t pid) {

    scheduler_adt scheduler = getSchedulerADT();

    if (scheduler->processes[pid] == NULL) {
        printf("Process %llu not found\n", pid);
        return -1;
    }

    //scheduler->processes[pid]->state = READY;
    add_process_to_list(scheduler->processes[pid]);
    return 0;
}



