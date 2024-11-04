#include <pipes.h>
#include <memman.h>
#include <io.h>
#include <scheduler.h>
//#include <syscalls.h>

typedef struct pipe_t
{
    uint16_t pipe_id;
    uint16_t fds[PIPE_FD]; //esta parte deberia estar en pipes o processes
    char * buffer[PIPE_BUFFER_SIZE];
    uint16_t buffer_count;
    uint16_t opened; // 0 if closed, 1 if open
    //Semaphores?
}pipe_t;

typedef struct pipe_manager_cdt
{
    pipe_t * pipes[MAX_PIPES];
    uint16_t pipe_count;
    uint16_t next_pipe_id;
    

};

typedef enum pipe_state {

    CLOSED=0,
    OPENED=1

} pipe_state;

static pipe_t * get_pipe(uint16_t pipe_id);
static pipe_manager_adt get_pipe_manager();
static uint16_t get_next_pipe_id();
static uint16_t switch_pipe_state(pipe_state state, uint16_t pipe_id);

pipe_manager_adt init_pipe_manager(){

    pipe_manager_adt pipe_manager = (pipe_manager_adt) PIPE_MANAGER_ADDRESS;
    pipe_manager->pipe_count = 0;
    return pipe_manager;

}

static pipe_manager_adt get_pipe_manager(){
    
    return (pipe_manager_adt) PIPE_MANAGER_ADDRESS;

}

static uint16_t get_next_pipe_id(){

    pipe_manager_adt pipe_manager = get_pipe_manager();
    if (pipe_manager->next_pipe_id >= MAX_PIPES){
        ker_write("Pipe manager is full\n");
        return -1;
    }
    return pipe_manager->next_pipe_id++;

}

uint16_t create_pipe(int fds[PIPE_FD]){

    pipe_manager_adt pipe_manager = get_pipe_manager();
    pipe_t * pipe = mem_alloc(sizeof(struct pipe_t));
    pipe->pipe_id = get_next_pipe_id();
    pipe->buffer_count = 0;
    pipe_manager->pipes[pipe->pipe_id] = pipe;
    pipe->fds[0] = fds[0];
    pipe->fds[1] = fds[1];
    return pipe->pipe_id;

}

static uint16_t switch_pipe_state(pipe_state state, uint16_t pipe_id){

    pipe_t * pipe = get_pipe(pipe_id);
    pipe->opened = state;
    return 0;

}

static pipe_t * get_pipe(uint16_t pipe_id){

    pipe_manager_adt pipe_manager = get_pipe_manager();
    pipe_t * pipe = pipe_manager->pipes[pipe_id];
    if (pipe == NULL){
        ker_write("Pipe not found\n");
        return NULL;
    }
    return pipe;

}

uint16_t open_pipe(uint16_t pipe_id){

    return switch_pipe_state(OPENED, pipe_id);

}

uint16_t close_pipe(uint16_t pipe_id){

   return switch_pipe_state(CLOSED, pipe_id);

}

uint16_t write_pipe(uint16_t pipe_id, char * data, uint16_t size){

    pipe_t * pipe = get_pipe(pipe_id);
    if (size > PIPE_BUFFER_SIZE){
        ker_write("Pipe buffer overflow\n");
        return -1;
    }
    if (pipe->opened == 0){
        ker_write("Pipe is closed\n");
        return -1;
    }
    if (pipe->buffer_count + size > PIPE_BUFFER_SIZE){
        ker_write("Pipe buffer overflow\n");
        return -1;
    }
    //TODO: 
    if (pipe->fds[1] == 0 || pipe->fds[1] == 1){
    sys_write(pipe->fds[1], data, size, 0xFF, 0x00);
    }
    else {

    }

}

uint16_t read_pipe(uint16_t pipe_id, uint16_t size){

    pipe_t * pipe = get_pipe(pipe_id);
    if (size > PIPE_BUFFER_SIZE){
        ker_write("Pipe buffer overflow\n");
        return -1;
    }

    sys_read(pipe->fds[0], pipe->buffer, size);

}








