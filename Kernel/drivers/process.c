// #include "../include/create_process.h"
// #include "../include/memman.h"
// #include "../include/syscalls.h"
// #include "../include/stdio.h"


#include "../include/process.h"
#include "../include/linkedListDT.h"
#include <string.h>


static char **alloc_args(char **args, uint64_t argc);

void init_process(process_t *process, uint16_t pid, Function code, 
                char **args, uint64_t argc, char *name, priority_t priority, 
                uint8_t unkilliable) {
    process->pid = pid;
    process->priority = priority;
    process->state = READY;
    process->stack_base = mem_alloc(STACK_SIZE);
    process->argv = alloc_args(args, argc);
    process->argc = argc;
    process->name = mem_alloc(strlen(name) + 1);
    strcpy(process->name, name);
    void *stack_end = (void *) ((uint64_t)process->stack_base + STACK_SIZE);

    process->stack_pointer = create_process_stack_frame((void *) code, stack_end, (void *) process->argv);
    process->unkilliable = unkilliable;

    }

static char **alloc_args(char **args, uint64_t argc) {
    char **argv = (char **) mem_alloc(sizeof(char **) * (argc + 1));
    for (int i = 0; i < argc; i++) {
        argv[i] = mem_alloc(strlen(args[i]) + 1);
        strcpy(argv[i], args[i]);
    }
    argv[argc] = NULL;
    return argv;
}


void free_process(process_t * process){
    for (int i = 0; i < process->argc; i++) {
        mem_free(process->argv[i]);
    }
    mem_free(process->argv);
    mem_free(process->name);
    mem_free(process->stack_base);
    mem_free(process);
}

process_amongus_t * get_process_amongus(process_t * process){
    process_amongus_t * process_amongus = (process_amongus_t *) mem_alloc(sizeof(process_amongus_t));
    process_amongus->pid = process->pid;
    process_amongus->priority = process->priority;
    process_amongus->state = process->state;
    process_amongus->stack_base = process->stack_base;
    process_amongus->stack_pointer = process->stack_pointer;
    process_amongus->argv = process->argv;
    process_amongus->argc = process->argc;
    process_amongus->name = mem_alloc(strlen(process->name) + 1);
    strcpy(process_amongus->name, process->name);
    return process_amongus;
}