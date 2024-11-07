// #include "../include/create_process.h"
#include "../include/memman.h"
// #include "../include/syscalls.h"
// #include "../include/stdio.h"

#include "../include/process.h"
#include "../include/linkedListADT.h"
#include "../include/lib.h"
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <scheduler.h>
#include <memman.h>


static char **alloc_args(char **args, uint64_t argc);

void init_process(process_t* process, int32_t pid, Function code,
                 char** args, uint64_t argc, char* name, priority_t priority, uint8_t unkilliable) {
    process->pid = pid;
    process->priority = priority;
    process->state = READY;
    process->stack_base = mem_alloc(STACK_SIZE);
    process->argv = alloc_args(args, argc);
    process->argc = argc;
    process->name = mem_alloc(strlen(name) + 1);
    memcpy(process->name, name, strlen(name) + 1);
    
    void* stack_end = (void*)((uint64_t)process->stack_base + STACK_SIZE);
    process->stack_pointer = create_process_stack_frame(
        (void*)code,
        stack_end,
        process->argv,
        argc,
        process_handler
    );
    
    process->unkilliable = unkilliable;

    process->parent_pid = get_current_pid();
    process->exit_code = 0;
    process->children = createLinkedList();

    process->fds[0] = STDIN;
    process->fds[1] = STDOUT;
    process->fds[2] = STDERR;
}

static char **alloc_args(char **args, uint64_t argc)
{
    char **argv = mem_alloc(sizeof(char *) * argc);
    for (int i = 0; i < argc; i++)
    {
        argv[i] = mem_alloc(strlen(args[i]) + 1);
        memcpy(argv[i], args[i], strlen(args[i]) + 1);
    }
    return argv;
}

void process_handler(Function code, char** argv, int argc) {
    int64_t ret = code(argc, argv);
    
    process_t* current = get_current_process();
    current->exit_code = ret;
    
    kill_current_process();
    
}

void free_process(process_t* process) {
    // Free arguments
    for (int i = 0; i < process->argc; i++) {
        mem_free(process->argv[i]);
    }
    mem_free(process->argv);
    
    mem_free(process->name);
    
    mem_free(process->stack_base);
    
    if (process->parent_pid != -1) {
        process_t* parent = get_process_by_pid(process->parent_pid);
        if (parent != NULL) {
            remove_child_process(parent, process->pid);
        }
    }
    
    // Free children list
    destroyLinkedList(process->children);
    
    // Finally free the process struct itself
    mem_free(process);
}

void remove_child_process(process_t* parent, int32_t child_pid) {
    node_t* current = getFirstNode(parent->children);
    while (current != NULL) {
        int32_t* current_pid = (int32_t*)current->process;
        if (*current_pid == child_pid) {
            removeNode(parent->children, current_pid);
            mem_free(current_pid);
            break;
        }
        current = current->next;
    }
}



process_amongus_t *get_process_amongus(process_t *process)
{
    process_amongus_t *process_amongus = (process_amongus_t *)mem_alloc(sizeof(process_amongus_t));
    process_amongus->pid = process->pid;
    process_amongus->priority = process->priority;
    process_amongus->state = process->state;
    process_amongus->stack_base = process->stack_base;
    process_amongus->stack_pointer = process->stack_pointer;
    process_amongus->argv = process->argv;
    process_amongus->argc = process->argc;
    process_amongus->name = mem_alloc(strlen(process->name) + 1);
    memcpy(process_amongus->name, process->name, strlen(process->name) + 1);
    return process_amongus;
}