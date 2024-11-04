#ifndef PIPES

#define MAX_PIPES 10
#define PIPE_MANAGER_ADDRESS 0x80000000
#define PIPE_BUFFER_SIZE 1024
#define PIPE_FD 2

typedef struct pipe_manager_cdt * pipe_manager_adt;

pipe_manager_adt init_pipe_manager();
uint16_t create_pipe();
uint16_t open_pipe(uint16_t pipe_id);
uint16_t close_pipe(uint16_t pipe_id);
uint16_t write_pipe(uint16_t pipe_id, char * data, uint16_t size);
uint16_t read_pipe(uint16_t pipe_id, char * data, uint16_t size);


#endif