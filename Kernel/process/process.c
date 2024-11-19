// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/process.h"
#include "../include/linked_list_adt.h"
#include "../include/lib.h"
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <scheduler.h>
#include <memman.h>

static char **alloc_args(char **args, uint64_t argc);

void init_process(process_t* process, int32_t pid, function code,
                 char** args, uint64_t argc, char* name, priority_t priority, uint8_t unkilliable) {
    process->pid = pid;
    process->priority = priority;
    process->state = ready;
    process->stack_base = b_alloc(stack_size);
    process->argv = alloc_args(args, argc);
    process->argc = argc;
    process->name = b_alloc(strlen(name) + 1);
    memcpy(process->name, name, strlen(name) + 1);
    
    void* stack_end = (void*)((uint64_t)process->stack_base + stack_size);
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
    process->children = create_linked_list();

    process->fds[0] = STDIN;
    process->fds[1] = STDOUT;
    process->fds[2] = STDERR;

    process->fg = foreground;
}



static char **alloc_args(char **args, uint64_t argc)
{
    char **argv = b_alloc(sizeof(char *) * argc);
    for (int i = 0; i < argc; i++)
    {
        argv[i] = b_alloc(strlen(args[i]) + 1);
        memcpy(argv[i], args[i], strlen(args[i]) + 1);
    }
    return argv;
}

void process_handler(function code, char** argv, int argc) {
    int64_t ret = code(argc, argv);
    
    process_t* current = get_current_process();
    current->exit_code = ret;
    
    kill_current_process();
    
}

void free_process(process_t* process) {
    // free arguments
    for (int i = 0; i < process->argc; i++) {
        b_free(process->argv[i]);
    }

    b_free(process->argv);
    
    b_free(process->name);
    
    b_free(process->stack_base);
    
    
    if (process->children != NULL && process->parent_pid != 0) {
        start_iterator(process->children);
        while (has_next(process->children)) {
            process_t* child = (process_t*)get_next(process->children);
            child->parent_pid = 0;
        }
    }
    
 
    destroy_linked_list(process->children);
    
  
    b_free(process);
}

void remove_child_process(process_t* parent, int32_t child_pid) {
   
}

process_snapshot_t *get_process_snapshot(uint32_t pid)
{
    process_t *process = get_process_by_pid(pid);
    if (process == NULL || process->pid < 0 || process->pid >MAX_PROCESSES) {
        return NULL;
    }

    process_snapshot_t *process_snapshot = (process_snapshot_t *)b_alloc(sizeof(process_snapshot_t));
    process_snapshot->pid = process->pid;
    process_snapshot->priority = process->priority;
    process_snapshot->state = process->state;
    process_snapshot->argv = process->argv;
    process_snapshot->argc = process->argc;
    process_snapshot->fg = process->fg;
    process_snapshot->name = b_alloc(strlen(process->name) + 1);
    memcpy(process_snapshot->name, process->name, strlen(process->name) + 1);
    process_snapshot->stack_pointer = b_alloc(20);
    pointer_to_string(process->stack_pointer, process_snapshot->stack_pointer, 20);
    process_snapshot->base_pointer = b_alloc(20);
    pointer_to_string(process->stack_base, process_snapshot->base_pointer, 20);
    return process_snapshot;
}

