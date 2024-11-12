  #ifndef pipes_h
  #define pipes_h

#define max_pipes 10
#define pipe_manager_address 0x2000000
#define pipe_buffer_size 1024

typedef struct pipe_manager_cdt * pipe_manager_adt;

#include <stdint.h>

pipe_manager_adt init_pipe_manager();
uint16_t create_pipe();
uint16_t open_pipe(uint16_t pid, uint16_t pipe_id, uint8_t mode);
uint16_t close_pipe(uint16_t pipe_id);
uint16_t write_pipe(uint16_t pid, uint16_t pipe_id, const char * data, uint16_t size);
uint16_t read_pipe(uint16_t pid, uint16_t pipe_id, char * data, uint16_t size);


#endif
