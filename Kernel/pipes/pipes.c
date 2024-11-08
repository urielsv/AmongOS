#include <pipes.h>
#include <memman.h>
#include <io.h>
#include <scheduler.h>
#include <definitions.h>

typedef struct pipe_t
{
    uint16_t pipe_id;
    char buffer[PIPE_BUFFER_SIZE];
    uint16_t buffer_count;
    uint16_t start_position;
    int16_t input_pid, output_pid;
    uint16_t opened;
    
}pipe_t;

typedef struct pipe_manager_cdt
{
    pipe_t * pipes[MAX_PIPES];
    uint16_t pipe_count;
    uint16_t next_pipe_id;

} pipe_manager_cdt;

#define buffer_position(pipe) (((pipe)->buffer_count + (pipe)->start_position) % PIPE_BUFFER_SIZE)

static pipe_t * get_pipe(uint16_t pipe_id);
static pipe_manager_adt get_pipe_manager();
static uint16_t get_next_pipe_id();
static void free_pipe(uint16_t pipe_id);


pipe_manager_adt init_pipe_manager() {

    pipe_manager_adt pipe_manager = (pipe_manager_adt) PIPE_MANAGER_ADDRESS;
    for (uint16_t i = BUILTIN_FDS; i < MAX_PIPES; i++){
        pipe_manager->pipes[i] = NULL;
    }

    //salteo los primero 3 casos, no lo inicilizo en null para que no lo tome como next_pipe_id
    pipe_manager->pipes[STDIN] = (pipe_t *) mem_alloc(sizeof(pipe_t));
    pipe_manager->pipes[STDOUT] = (pipe_t *) mem_alloc(sizeof(pipe_t));
    pipe_manager->pipes[STDERR] = (pipe_t *) mem_alloc(sizeof(pipe_t));

    pipe_manager->next_pipe_id = BUILTIN_FDS;
    pipe_manager->pipe_count = BUILTIN_FDS;
    return pipe_manager;

}

static pipe_manager_adt get_pipe_manager(){
   
   return (pipe_manager_adt) PIPE_MANAGER_ADDRESS;

}

static uint16_t get_next_pipe_id(){

    pipe_manager_adt pipe_manager = get_pipe_manager();
    if (pipe_manager->pipe_count >= MAX_PIPES){
        ker_write("Pipe manager is full\n");
        return -1;
    }
    while (pipe_manager->pipes[pipe_manager->next_pipe_id] != NULL){
        pipe_manager->next_pipe_id = (pipe_manager->next_pipe_id + 1) % MAX_PIPES;
    }
    return pipe_manager->next_pipe_id;

}

uint16_t create_pipe(){

    pipe_manager_adt pipe_manager = get_pipe_manager();
    pipe_t * pipe = mem_alloc(sizeof(struct pipe_t));
    pipe->pipe_id = get_next_pipe_id();
    pipe->buffer_count = 0;
    pipe_manager->pipes[pipe->pipe_id] = pipe;
    pipe->opened = CLOSED;
    pipe->input_pid = -1;
    pipe->output_pid = -1;
    pipe_manager->pipe_count++;
    return pipe->pipe_id;

}

static pipe_t * get_pipe(uint16_t pipe_id){

    pipe_manager_adt pipe_manager = get_pipe_manager();
    pipe_t * pipe = pipe_manager->pipes[pipe_id];
    if (pipe == NULL || pipe_id < BUILTIN_FDS){
        ker_write("Pipe not found\n");
        return NULL;
    }
    return pipe;

}

uint16_t open_pipe(uint16_t pipe_id, uint8_t mode){

    pipe_t * pipe = get_pipe(pipe_id);
    if (pipe == NULL){
        ker_write("Pipe not found\n");
        return -1;
    }
    pipe->opened = OPENED;
    if (mode == READ_MODE && pipe->input_pid == -1){
        pipe->input_pid = get_current_pid();
    }
    else if (mode == WRITE_MODE && pipe->output_pid == -1){
        pipe->output_pid = get_current_pid();
    }
    return 0;

}

uint16_t close_pipe(uint16_t pipe_id){


    pipe_t * pipe = get_pipe(pipe_id);
    if (pipe == NULL){
        ker_write("Pipe not found\n");
        return -1;
    }
    pipe->opened = CLOSED;
    uint16_t current_pid = get_current_pid();

    if (pipe->input_pid == current_pid){
        char eofString[1] = {EOF};
	write_pipe(current_pid, pipe_id, eofString, 1);
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
    mem_free(pipe);
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
		if (pipe->buffer_count >= PIPE_BUFFER_SIZE) {
			pipe->opened = CLOSED;
			block_process(pipe->input_pid);
			// yield();
		}

		while (pipe->buffer_count < PIPE_BUFFER_SIZE && written_bytes < size) {
			pipe->buffer[buffer_position(pipe)] = data[written_bytes];
			if ((int) data[written_bytes++] == EOF)
				break;
			pipe->buffer_count++;
		}
		if (pipe->opened == CLOSED) {
			unblock_process((uint16_t) pipe->output_pid);
			pipe->opened = OPENED;
		}
	}
	return written_bytes;



}

uint16_t read_pipe(uint16_t pid, uint16_t pipe_id, char * data, uint16_t size){

    pipe_t * pipe = get_pipe(pipe_id);
    
    if ( size == 0 || data == NULL || pipe==NULL|| pipe->output_pid != pid){
        return -1;
    }

    uint8_t eof_read = 0;
	uint64_t read_bytes = 0;
	while (read_bytes < size && !eof_read) {
		if (pipe->buffer_count == 0 && (int) pipe->buffer[pipe->start_position] != EOF) {
			pipe->opened = CLOSED;
			block_process(pipe->output_pid);
			// yield(); //ya se hace yield en block process
		}
		while ((pipe->buffer_count > 0 || (int) pipe->buffer[pipe->start_position] == EOF) && read_bytes < size) {
			data[read_bytes] = pipe->buffer[pipe->start_position];
			if ((int) data[read_bytes++] == EOF) {
				eof_read = 1;
				break;
			}
			pipe->buffer_count--;
			pipe->start_position = (pipe->start_position + 1) % PIPE_BUFFER_SIZE;
		}
		if (pipe->opened == CLOSED) {
			unblock_process(pipe->input_pid);
			pipe->opened = OPENED;
		}
	}
	return read_bytes;


}









