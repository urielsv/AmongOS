#ifndef _CREATE_PROCESS_H
#define _CREATE_PROCESS_H

#include <stdint.h>
#include <definitions.h>
#include "linkedListADT.h"



typedef struct process_t {
    int32_t pid;
    priority_t priority;
    state_t state;
    void* stack_base;
    void* stack_pointer;
    char** argv;
    uint64_t argc;
    char* name;
    
    uint8_t unkilliable;
    int32_t parent_pid;
    int32_t exit_code;
    //uint8_t has_been_waited;
    
    // Children management
    linkedListADT children;      
    //int32_t waiting_for_pid;    
} process_t;

typedef int (*Function)(int argc, char **args);

//snapshot xD
typedef struct process_amongus_t { 
    uint32_t pid;
    uint16_t priority;
    uint16_t state;
    void * stack_base;
    void * stack_pointer;
    char ** argv;
    uint64_t argc;
    char *name;
}process_amongus_t;


typedef int (*Function)(int argc, char** args);

void process_handler(Function code, char** argv, int argc);
void init_process(process_t* process, int32_t pid, Function code, char** args, uint64_t argc, char* name, priority_t priority, uint8_t unkilliable);
extern void* create_process_stack_frame(void* rip, void* rsp, void* argv, uint64_t argc, void* process_handler);
void free_process(process_t* process);
void add_child_process(process_t* parent, int32_t child_pid);
void remove_child_process(process_t* parent, int32_t child_pid);
int has_children(process_t* process);
int is_child_of(process_t* potential_parent, int32_t child_pid);

#endif