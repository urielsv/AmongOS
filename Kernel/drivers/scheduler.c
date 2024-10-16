#include "../include/scheduler.h"
#include "../include/memman.h"
#include "../include/process.h"
#include "../include/linkedListADT.h"
#include <io.h>
#define IDLE_PID 0


typedef struct scheduler_cdt {

    node_t * processes[MAX_PROCESSES];
    linkedListADT blocked_process_list;
	linkedListADT process_list;
	uint16_t current_pid;
	uint16_t next_unused_pid;
	uint16_t remaining_processes;
} scheduler_cdt;


scheduler_adt init_scheduler() {

    scheduler_adt scheduler = (scheduler_adt) SCHEDULER_ADDRESS;

    scheduler->process_list = createLinkedList();
    scheduler->blocked_process_list = createLinkedList();

    for (int i = 0; i < MAX_PROCESSES; i++) {
        scheduler->processes[i] = NULL;
    }

    scheduler->next_unused_pid = 0;
    return scheduler;
}

scheduler_adt getSchedulerADT() {
	return (scheduler_adt) SCHEDULER_ADDRESS;
}

//Agrega o remueve nodos de la lista de punteros a procesos segun la prioridad.
int32_t set_priority(uint16_t pid, uint8_t new_priority) {

    scheduler_adt scheduler = getSchedulerADT();
    process_t * process_pointer = scheduler->processes[pid]->process;

    if (process_pointer == NULL || pid == IDLE_PID || new_priority > HIGH)
		return -1;

    int old_priority = process_pointer->priority;
    process_pointer->priority = new_priority;

    if (old_priority != new_priority) {
        int difference = new_priority - old_priority;

        if (difference > 0) { 
            for (int i = 0; i < difference; i++) {
                addNode(scheduler->process_list, (void *) process_pointer);
            }
        } else { 
            for (int i = 0; i < -difference; i++) {
                removeNode(scheduler->process_list, (void *) process_pointer);
            }
        }
    }

    return new_priority;

}

uint16_t get_next_pid(scheduler_adt scheduler) {
    process_t *process = (process_t *) getNextNode(scheduler->process_list);
	if (process == NULL)
		return IDLE_PID;
	return process->pid;
}


void* scheduler(void* stack_pointer) {

    // while(1) {
    //     ker_write_color("asdasda hola", 0x00FF00, 0x00);
    // };

    static int firstTime = 1;
    scheduler_adt scheduler = getSchedulerADT();

    if (!scheduler->remaining_processes) {
        return stack_pointer;
    }

    //quiere decir que el proceso actual ya no puede seguir corriendo.

    process_t *current_process;
	node_t *current_process_node = scheduler->processes[scheduler->current_pid];

    if (current_process_node != NULL) {
        current_process = (process_t *) current_process_node->process;
		if (!firstTime)
			current_process->stack_pointer = stack_pointer;
		else
			firstTime = 0;
		if (current_process->state == RUNNING)
			current_process->state = READY;
    }

    scheduler->current_pid = get_next_pid(scheduler);
    current_process = scheduler->processes[scheduler->current_pid]->process;

    current_process->state = RUNNING;
    return current_process->stack_pointer;
}


int16_t create_process(Function code, char **args, int argc, char *name, uint8_t priority, uint8_t unkillable) {

    scheduler_adt scheduler = getSchedulerADT();

    if (scheduler->remaining_processes >= MAX_PROCESSES) {
        ker_write("Max processes reached\n");
        return -1;
    }

    process_t *process = (process_t *) mem_alloc(sizeof(process_t));

    if (process == NULL) {
        ker_write("Error creating process\n");
        return -1;
    }

    init_process(process, scheduler->next_unused_pid, code, args,argc, name, priority, unkillable);

    node_t *process_node;

    if (process->pid != IDLE_PID) {
        ker_write("Creating process\n");
        for (int i = 0; i < process->priority; i++) {
            addNode(scheduler->process_list, (void *) process);
            
        }
    }
    else {
        ker_write("Creating idle process\n");
		process_node = mem_alloc(sizeof(node_t));
		process_node->process = (void *) process;
	}
    
	scheduler->processes[process->pid] = process_node;

    while (scheduler->processes[scheduler->next_unused_pid] != NULL)
        scheduler->next_unused_pid = (scheduler->next_unused_pid + 1) % MAX_PROCESSES;

    scheduler->remaining_processes++;

    return process->pid;
}


int kill_process(uint16_t pid) {

    scheduler_adt scheduler = getSchedulerADT();
    if(pid == IDLE_PID) {
        ker_write("Cannot kill idle process\n");
    }
    if (scheduler==NULL) {
        ker_write("Scheduler not initialized\n");
        return -1;
    }

    if (scheduler->processes[pid] == NULL) {
        ker_write("Process %llu not found\n", pid);
        return -1;
    }

    process_t *process_to_kill = (process_t *) scheduler->processes[pid]->process;

    if (process_to_kill->unkilliable)
		return -1;

	// TODO: falta implementar files descriptors 
    if (process_to_kill->state == BLOCKED) {
        removeNode(scheduler->blocked_process_list, process_to_kill);
    }
    else {
        for (int i = 0; i < process_to_kill->priority; i++) {
            removeNode(scheduler->process_list, process_to_kill);
        }
    }

    return 0;
}

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



