#ifndef _CREATE_PROCESS_H
#define _CREATE_PROCESS_H



//puntero a la funcion que se va a ejecutar
// typedef int (*Function)(int argc, char **args);

// typedef struct {

//     uint64_t pid;
//     uint16_t priority;
//     uint16_t state;
//     void * stack_base;
//     void * stack_pointer;
//     char ** argv;
//     uint64_t argc;

// } process_t;

// #define MAX_PROCESSES 10
// #define STACK_SIZE (1<<12)

// enum priority { LOW, LOW_MEDIUM, MEDIUM, HIGH_MEDIUM, HIGH };

// int create_process(Function code, int argc, char **argv);
// int kill_process(uint64_t pid);
// int block_process(uint64_t pid);
// int unblock_process(uint64_t pid);
// extern void * create_process_stack_frame(void * rip, void * rsp, void * argv);

#endif