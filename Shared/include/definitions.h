#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define MAX_PROCESSES (1<<12)
#define STACK_SIZE (1<<12)
#define SCHEDULER_ADDRESS 0x600000
#define MAX_PRIORITY 5
#define EOF (-1)

typedef enum { BLOCKED = 0, READY, KILLED, RUNNING} state_t;
typedef enum { LOW = 1, LOW_MEDIUM, MEDIUM, HIGH_MEDIUM, HIGH } priority_t;

typedef int (*Function)(int argc, char **args);

#endif