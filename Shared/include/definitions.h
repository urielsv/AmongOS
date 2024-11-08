#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define MAX_PROCESSES (1<<12)
#define STACK_SIZE (1<<12)
#define SCHEDULER_ADDRESS 0x600000
#define MAX_PRIORITY 5
#define EOF (-1)

#define DEFAULT_PRIORITY MEDIUM

typedef enum { BLOCKED = 0, READY, KILLED, RUNNING, WAITING_FOR_CHILD} state_t;
typedef enum { LOW = 1, LOW_MEDIUM, MEDIUM, HIGH_MEDIUM, HIGH } priority_t;
typedef enum { STDIN=0, STDOUT, STDERR} basic_fd_t;
typedef enum pipe_state { CLOSED=0, OPENED=1 } pipe_state;

typedef enum pipe_mode { READ_MODE=0, WRITE_MODE=1 } pipe_mode;


typedef int (*Function)(int argc, char **args);

typedef struct {
    int32_t pid;
    priority_t priority;
    state_t state;
    char* name;
    int argc;
    char** argv;
} process_snapshot_t;

#endif
