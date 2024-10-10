#include "../include/create_process.h"
#include "../include/memman.h"
#include "../include/syscalls.h"
#include "../include/stdio.h"

#define MAX_PROCESSES 10
#define STACK_SIZE (1<<12)

enum State { BLOCKED, READY, EXITED };
enum priority { LOW, LOW_MEDIUM, MEDIUM, HIGH_MEDIUM, HIGH };



typedef struct {

    uint64_t pid;
    uint16_t priority;
    enum State state;
    void * stack_base;
    void * stack_pointer;
    char ** argv;
    uint64_t argc;

} process_t;

int process_count = 0;

int create_process(Function code, int argc, char **argv) {

    if (process_count >= MAX_PROCESSES) {
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
    process->state = READY;
    process->priority = LOW;
    process->stack_pointer = create_process_stack_frame ((void *) code, (void *) ((uint64_t) process->stack_base + STACK_SIZE), (void *) argv);
    process_count++;

    return process->pid;
}

int kill_process(uint64_t pid) {

    return 0;
}

int block_process(uint64_t pid) {

    return 0;
}

int unblock_process(uint64_t pid) {

    return 0;
}

