#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stdio.h>
#define MAX_PROCESSES (1<<12)
#define STACK_SIZE (1<<12)
#define SCHEDULER_ADDRESS 0x60000

typedef int (*Function)(int argc, char **args);
typedef struct scheduler_cdt* scheduler_adt;

int create_process(Function code, int argc, char **argv);
int kill_process(uint64_t pid);
int block_process(uint64_t pid);
int unblock_process(uint64_t pid);
extern void * create_process_stack_frame(void * rip, void * rsp, void * argv);

void* scheduler(void* stack_pointer);

#endif // SCHEDULER_H