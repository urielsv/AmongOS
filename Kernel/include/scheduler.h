#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stdio.h>
#include <definitions.h>
#include <process.h>

typedef int (*Function)(int argc, char **args);
typedef struct scheduler_cdt* scheduler_adt;

extern void asm_do_timer_tick();

int32_t create_process(Function code, char **args, int argc, char *name, uint8_t priority, uint8_t unkillable);
int kill_process(uint32_t pid);
int block_process(uint64_t pid);
int unblock_process(uint64_t pid);
scheduler_adt init_scheduler();
int32_t get_next_ready_pid();
void kill_current_process();
void yield();
void* scheduler(void* stack_pointer);
void process_priority(uint64_t pid, uint8_t new_prio);
uint32_t get_current_pid();
process_t * get_current_process();
void waitpid(uint32_t child_pid);
process_t *get_process_by_pid(uint32_t pid);
int16_t get_current_process_file_descriptor(uint8_t fd_index);
uint16_t change_process_fd(uint32_t pid, uint16_t fd_index, uint16_t new_fd);

#endif // SCHEDULER_H
