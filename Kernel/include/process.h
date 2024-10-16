#ifndef _CREATE_PROCESS_H
#define _CREATE_PROCESS_H

#include <stdint.h>
#include <definitions.h>
#include "linkedListADT.h"



typedef struct {
    uint64_t pid;
    priority_t priority;
    state_t state;

    void * stack_base;
    void * stack_pointer;

    char ** argv;
    uint64_t argc;
    char *name;
    
    uint8_t unkilliable;
    linkedListADT children;
} process_t;

typedef int (*Function)(int argc, char **args);

//snapshot xD
typedef struct process_amongus_t { 
    uint64_t pid;
    uint16_t priority;
    uint16_t state;
    void * stack_base;
    void * stack_pointer;
    char ** argv;
    uint64_t argc;
    char *name;
}process_amongus_t;


void init_process(process_t *process, uint16_t pid, Function code, char **args, uint64_t argc, char *name, priority_t priority, uint8_t unkilliable);
extern void * create_process_stack_frame(void * rip, void * rsp, void * argv);

#endif