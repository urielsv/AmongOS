// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <pipes.h>
#include <io.h>
#include <scheduler.h>
#include <definitions.h>
#include <lib.h>
#include <memman.h>

typedef struct pipe_t
{
    uint16_t pipe_id;
    char buffer[pipe_buffer_size];
    uint16_t buffer_count;
    uint16_t start_position;
    int16_t input_pid, output_pid;
    uint16_t opened;
    
} pipe_t;

typedef struct pipe_manager_cdt
{
    pipe_t * pipes[max_pipes];
    uint16_t pipe_count;
    uint16_t next_pipe_id;

} pipe_manager_cdt;

#define buffer_position(pipe) (((pipe)->buffer_count + (pipe)->start_position) % pipe_buffer_size)

static pipe_t * get_pipe(uint16_t pipe_id);
static pipe_manager_adt get_pipe_manager();
static uint16_t get_next_pipe_id();
static void free_pipe(uint16_t pipe_id);


pipe_manager_adt init_pipe_manager() {

    pipe_manager_adt pipe_manager = (pipe_manager_adt) pipe_manager_address;
    for (uint16_t i = BUILTIN_FDS; i < max_pipes; i++){
        pipe_manager->pipes[i] = NULL;
    }

    pipe_manager->pipes[STDIN] = (pipe_t *) b_alloc(sizeof(pipe_t));
    pipe_manager->pipes[STDOUT] = (pipe_t *) b_alloc(sizeof(pipe_t));
    pipe_manager->pipes[STDERR] = (pipe_t *) b_alloc(sizeof(pipe_t));

    pipe_manager->next_pipe_id = BUILTIN_FDS;
    pipe_manager->pipe_count = BUILTIN_FDS;
    return pipe_manager;

}

static pipe_manager_adt get_pipe_manager(){
   
   return (pipe_manager_adt) pipe_manager_address;

}

static uint16_t get_next_pipe_id(){

    pipe_manager_adt pipe_manager = get_pipe_manager();
    if (pipe_manager->pipe_count >= max_pipes){
        ker_write("pipe manager is full\n");
        return -1;
    }
    while (pipe_manager->pipes[pipe_manager->next_pipe_id] != NULL){
        pipe_manager->next_pipe_id = (pipe_manager->next_pipe_id + 1) % max_pipes;
    }
    return pipe_manager->next_pipe_id;

}

uint16_t create_pipe() {

    pipe_manager_adt pipe_manager = get_pipe_manager();
    pipe_t * pipe = b_alloc(sizeof(struct pipe_t));
    pipe->pipe_id = get_next_pipe_id();
    pipe->buffer_count = 0;
    pipe_manager->pipes[pipe->pipe_id] = pipe;
    pipe->opened = closed;
    pipe->input_pid = -1;
    pipe->output_pid = -1;
    pipe_manager->pipe_count++;
    return pipe->pipe_id;

}

static pipe_t * get_pipe(uint16_t pipe_id){

    pipe_manager_adt pipe_manager = get_pipe_manager();
    pipe_t * pipe = pipe_manager->pipes[pipe_id];
    if (pipe == NULL || pipe_id < BUILTIN_FDS){
        ker_write("pipe not found\n");
        return NULL;
    }
    return pipe;

}

uint16_t open_pipe(uint16_t pid, uint16_t pipe_id, uint8_t mode){

    pipe_t * pipe = get_pipe(pipe_id);
    if (pipe == NULL){
        ker_write("pipe not found\n");
        return -1;
    }
    pipe->opened = opened;
    if (mode == read_mode && pipe->output_pid == -1){
        pipe->output_pid = pid;
    }
    if (mode == write_mode && pipe->input_pid == -1){
        pipe->input_pid = pid;
    }
    return 0;

}

uint16_t close_pipe(uint16_t pipe_id){

    pipe_t * pipe = get_pipe(pipe_id);
    if (pipe == NULL){
        ker_write("pipe not found\n");
        return -1;
    }
    pipe->opened = closed;
    uint16_t current_pid = get_current_pid();

    if (pipe->input_pid == current_pid){
        char eof_string[1] = {EOF};
	write_pipe(current_pid, pipe_id, eof_string, 1);
    }
    else if (pipe->output_pid == current_pid){
        free_pipe(pipe_id);
    }
    else return -1;

    return 0;

}


static void free_pipe(uint16_t pipe_id){

    pipe_manager_adt pipe_manager = get_pipe_manager();
    pipe_t * pipe = pipe_manager->pipes[pipe_id];
    b_free(pipe);
    pipe_manager->pipes[pipe_id] = NULL;
    pipe_manager->pipe_count--;

}

uint16_t write_pipe(uint16_t pid, uint16_t pipe_id, const char * data, uint16_t size){

    pipe_t * pipe = get_pipe(pipe_id);

    if (size == 0 || data == NULL || pipe==NULL|| pipe->input_pid != pid){
        return -1;
    }

    uint64_t written_bytes = 0;
	while (written_bytes < size && (int) pipe->buffer[buffer_position(pipe)] != EOF) {
		if (pipe->buffer_count >= pipe_buffer_size) {
			pipe->opened = closed;
			block_process(pipe->input_pid);
			yield();
		}

		while (pipe->buffer_count < pipe_buffer_size && written_bytes < size) {
			pipe->buffer[buffer_position(pipe)] = data[written_bytes];
			if ((int) data[written_bytes++] == EOF){
                break;
            }
			pipe->buffer_count++;
		}
		if (pipe->opened == closed) {
			unblock_process((uint16_t) pipe->output_pid);
			pipe->opened = opened;
		}
	}
	return written_bytes;



}

uint16_t read_pipe(uint16_t pid, uint16_t pipe_id, char * data, uint16_t size){


    pipe_t * pipe = get_pipe(pipe_id);
    
    if (  pipe == NULL || pipe->output_pid != pid){
        return -1;
    }

    uint8_t eof_read = 0;
	uint64_t read_bytes = 0;
	while (read_bytes < size && !eof_read) {
		if (pipe->buffer_count == 0 && (int) pipe->buffer[pipe->start_position] != EOF) {
			pipe->opened = closed;
			block_process(pipe->output_pid);
			yield();
		}
		while ((pipe->buffer_count > 0 || (int) pipe->buffer[pipe->start_position] == EOF) && read_bytes < size) {
			data[read_bytes] = pipe->buffer[pipe->start_position];
			if ((int) data[read_bytes++] == EOF) {
				eof_read = 1;
				break;
			}
			pipe->buffer_count--;
			pipe->start_position = (pipe->start_position + 1) % pipe_buffer_size;
		}
		if (pipe->opened == closed) {
			unblock_process(pipe->input_pid);
			pipe->opened = opened;
		}
	}
	return read_bytes;


}