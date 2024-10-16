#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stdio.h>
#define MAX_PROCESSES (1<<12)
#define STACK_SIZE (1<<12)
#define SCHEDULER_ADDRESS 0x600000

typedef int (*Function)(int argc, char **args);
typedef struct scheduler_cdt* scheduler_adt;

int16_t create_process(Function code, char **args, int argc, char *name, uint8_t priority, uint8_t unkillable);
int kill_process(uint64_t pid);
int block_process(uint64_t pid);
int unblock_process(uint64_t pid);
scheduler_adt init_scheduler();
void* scheduler(void* stack_pointer);

#endif // SCHEDULER_H