/******************************************************************************
 *
 * @file    syscalls.h
 *
 * @brief   handle system calls (kernel side) 
 *
 * @author  luca pugliese                           <lpugliese@itba.edu.ar>
 * @author  felipes venturino                        <fventurino@itba.edu.ar>
 * @author  uriel sosa vazquez                      <usosavazquez@itba.edu.ar>
 *
 ******************************************************************************/

#ifndef syscalls_h
#define syscalls_h

#include <stdint.h>
#include <stddef.h>
#include <process.h>
#include <stdlib.h>
#include <memman.h>

/*
 * @name sys_write
 *
 * @brief writes into the file descriptor the buffer passed as parameter.
 *
 * @param fd file descriptor.
 * @param buffer buffer to be written.
 * @param count amount of bytes to be written.
 * @param fgcolor foreground color.
 * @param bgcolor background color.
 * 
 */
int64_t sys_write(uint8_t fd, const char *buffer, uint64_t count, uint64_t fgcolor, uint64_t bgcolor);

/*
 * @name sys_read
 *
 * @brief reads from the buffer into the file descriptor passed as parameter.
 * @param fd file descriptor.
 * @param buffer buffer to be modified.
 * @param count amount of bytes to be read.
 */
char *sys_read(uint8_t fd, char *buffer, uint64_t count);


/* 
 * @name pid
 *
 * @brief returns the current process id. (wip)
 */
uint64_t pid();

/*
 * @name sys_exec
 *
 * @brief executes a program. (wip)
 */
void sys_exec();

/*
 * @name sys_ticks
 * @brief returns the current system tick count. 
 */
uint64_t sys_ticks();

/*
 * @name sys_read_char
 * @brief returns the last character read from the keyboard.
 */
char sys_read_char();

/*
 * @name sys_seconds
 * @brief returns the current system time in seconds.
 */
uint64_t sys_seconds();



/*
 * @name sys_random_number
 * @brief returns a random number based of the seed passed as parameter.
 */
uint64_t sys_random_number(int seed);

/*
 * @name draw
 * @brief draws a pixel (8x8) in the position passed as parameter.
 */
void draw(uint32_t color, uint64_t posx, uint64_t posy);

void sys_sleep(uint64_t millis);

/*
 * @name sys_time
 *
 * @brief returns the current system time in format
 * @returns char* with the current system time. (hh:mm:ss)
 * 
 */
char* sys_time();

/*
 * @name sys_sound
 * @brief plays a sound with the frequency and duration passed as parameters.
 *
 * @param freq frequency of the sound.
 * @param duration duration of the sound.
 * 
 */
void sys_sound(uint8_t freq, uint64_t duration);

/*
 * @name sys_hlt
 *
 * @brief halts the system until an interrupt is received.
 */
void sys_hlt();

/*
 * @name sys_clear
 *
 * @brief clears the screen with the color passed as parameter.
 * 
 * @param color color to clear the screen.
 */
void sys_clear(uint32_t color);

/*
 * @name sys_writing_position
 *
 * @brief sets the writing position to the coordinates passed as parameters.
 * 
 * @param x x coordinate.
 * @param y y coordinate.
 * 
 */
void sys_writing_position(uint64_t x, uint64_t y);

/*
 * @name screen_info
 *
 * @brief returns the screen width and height.
 * 
 * @param width pointer to the width variable.
 * @param height pointer to the height variable.
 */
void screen_info(uint64_t *width, uint64_t *height);

/*
 * @name font_size
 *
 * @brief sets the font size to the size passed as parameter. 
 * 
 * @param size size of the font.
 */
void font_size(int size);

/*
 * @name sys_registers
 *
 * @brief prints the registers of the last time captured.
 * 
 */
void sys_registers();

/*
 * @name save_registers
 *
 * @brief captures the values of registers at this time.
 */
void save_registers(uint64_t *stack);

/*
 * @name test_exc_zero
 *
 * @brief test exception zero.
 */

void test_exc_zero();

void test_exc_invalid_opcode();

void *sys_mem_alloc(size_t size);

void sys_mem_free(void *address);

void sys_create_process(function code, char **argv, int argc, char *name, uint8_t priority);

int sys_kill_process(uint64_t pid);

int sys_block_process(uint64_t pid);

int sys_unblock_process(uint64_t pid);

int sys_set_priority(uint64_t pid, uint8_t priority);

int sys_get_pid();

void sys_yield();

int sys_sem_open(int64_t id, int64_t initial_value);

void sys_sem_wait(int64_t id);

void sys_sem_post(int64_t id);

void sys_sem_close(int64_t id);

//uint32_t sys_waitpid(uint64_t pid, int *status);

void sys_waitpid(uint32_t child_pid);

uint16_t sys_create_pipe();

uint16_t sys_open_pipe(uint16_t pid, uint16_t pipe_id, uint8_t mode);

uint16_t sys_close_pipe(uint16_t pipe_id);

uint16_t sys_write_pipe(uint16_t pid, uint16_t pipe_id, char * data, uint16_t size);

uint16_t sys_read_pipe(uint16_t pid, uint16_t pipe_id, char * data, uint16_t size);

uint8_t sys_process_exists(uint32_t pid);

process_snapshot_t *sys_process_snapshot(uint32_t pid);

uint16_t sys_change_process_fd(uint32_t pid, uint16_t fd_index, int16_t new_fd);

size_t *sys_mem_info();

int sys_get_line_count();

void sys_reset_line_count();

#endif
