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

    if (process->parent_pid > 0) {
        process_t* parent = get_process_by_pid(process->parent_pid);
        if (parent != NULL) {
            if (parent->fds[0] != STDIN) process->fds[0] = parent->fds[0];
            if (parent->fds[1] != STDOUT) process->fds[1] = parent->fds[1];
            if (parent->fds[2] != STDERR) process->fds[2] = parent->fds[2];
        }
    }
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

process_snapshot_t *get_process_snapshot(uint32_t pid)
{
    process_t *process = get_process_by_pid(pid);
    if (process == NULL) {
        return NULL;
    }

    process_snapshot_t *process_snapshot = (process_snapshot_t *)mem_alloc(sizeof(process_snapshot_t));
    process_snapshot->pid = process->pid;
    process_snapshot->priority = process->priority;
    process_snapshot->state = process->state;
    process_snapshot->argv = process->argv;
    process_snapshot->argc = process->argc;
    process_snapshot->name = mem_alloc(strlen(process->name) + 1);
    memcpy(process_snapshot->name, process->name, strlen(process->name) + 1);
    return process_snapshot;
}
