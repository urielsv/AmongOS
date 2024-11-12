#ifndef definitions_h
#define definitions_h

#define max_processes (1<<5)
#define stack_size (1<<12)
#define scheduler_address 0x600000
#define max_priority 5
#define eof (-1)
#define builtin_fds 3
#define dev_NULL -1

#define default_priority medium

typedef enum { blocked = 0, ready, killed, running, waiting_for_child} state_t;
typedef enum { low = 1, low_medium, medium, high_medium, high } priority_t;
typedef enum { STDIN=0, STDOUT, STDERR} basic_fd_t;
typedef enum pipe_state { closed=0, opened=1 } pipe_state;

typedef enum pipe_mode { read_mode=0, write_mode=1 } pipe_mode;


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
