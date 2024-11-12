#ifndef definitions_h
#define definitions_h

#define MAX_PROCESSES (1<<5)
#define stack_size (1<<12)
#define SCHEDULER_ADDRESS 0x600000
#define MAX_PRIORITY 5
#define eof (-1)
#define BUILTIN_FDS 3
#define DEV_NULL -1

#define DEFAULT_PRIORITY medium

typedef enum { blocked = 0, ready, killed, running, waiting_for_child} state_t;
typedef enum { low = 1, low_medium, medium, high_medium, high } priority_t;
typedef enum { STDIN=0, STDOUT, STDERR } basic_fd_t;
typedef enum pipe_state { closed=0, opened } pipe_state;

typedef enum pipe_mode { read_mode=0, write_mode } pipe_mode;


typedef int (*function)(int argc, char **args);

typedef struct {
    int32_t pid;
    priority_t priority;
    state_t state;
    char* name;
    int argc;
    char** argv;
    char * stack_pointer;
    char * base_pointer;
} process_snapshot_t;

#endif
