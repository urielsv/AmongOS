/******************************************************************************
 *
 * @file    syscalls.h
 *
 * @brief   handle system calls (userland side) 
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
#include <definitions.h>

/*
 * @name write
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
extern int64_t write(uint8_t fd, const char *buffer, uint64_t count, uint64_t fgcolor, uint64_t bgcolor);

/*
 * @name read
 *
 * @brief reads from the buffer into the file descriptor passed as parameter.
 * @param fd file descriptor.
 * @param buffer buffer to be modified.
 * @param count amount of bytes to be read.
 */
extern int8_t * read(uint8_t fd, char *buffer, uint64_t count);


/*
 * @name read_char
 * @brief returns the last character read from the keyboard.
 */
extern int8_t read_char();


/*
 * @name sys_random_number
 * @brief returns a random number based of the seed passed as parameter.
 */
extern uint64_t random(int seed);

/*
 * @name draw
 * @brief draws a pixel (8x8) in the position passed as parameter.
 */
extern void draw(uint32_t color, uint64_t posx, uint64_t posy);

/*
 * @name time
 * @brief returns the current time (hh:mm:ss).
 */
extern char *time();

/*
 * @name sleep
 * @brief sleeps the system for the amount of milliseconds passed as parameter.
 * 
 */
extern uint64_t sleep(uint64_t millis);

/*
 * @name sound
 * @brief plays a sound with the frequency and duration passed as parameter.
 * @param freq frequency of the sound.
 * @param duration duration of the sound.
 * 
 */
extern void sound(uint64_t freq, uint64_t duration);

/*
 * @name hlt
 * @brief halts the system until an interrupt is received.
 *
 */
extern void hlt();

/*
 * @name clear
 * @brief clears the screen with the color passed as parameter.
 * 
 */
extern void clear(uint32_t color);

/*
 * @name writing_pos
 * @brief sets the position of the next character(s) to be written.
 * 
 */
extern void writing_pos(uint64_t x, uint64_t y);

/*
 * @name screen_info
 * @brief returns the screen width and height.
 * 
 * @param width pointer to the width variable.
 * @param height pointer to the height variable.
 * 
 */
extern void screen_info(uint64_t *width, uint64_t *height);

/* 
 * @name font_size
 * @brief sets the font size.
 * 
 */
extern void font_size(int size);

/*
 * @name sys_registers
 * @brief prints the registers.
 * 
 */
extern void sys_registers();


extern void test_exc_zero();

extern void test_exc_invalid_opcode();

extern void *mem_alloc(size_t size);
extern void mem_free(void *ptr);
extern int64_t exec(void *code, char **argv, int argc, char *name, uint8_t priority);
extern int kill(uint64_t pid);
extern int block(uint64_t pid);
extern int unblock(uint64_t pid);
extern int set_priority(uint64_t pid, uint8_t priority);
extern int get_pid();

extern void yield();
extern int32_t sem_open(int64_t id, int64_t initial_value);
extern void sem_wait(int64_t id);
extern void sem_post(int64_t id);
extern void sem_close(int64_t id);
extern void waitpid(uint32_t pid);
extern uint8_t process_exists(uint32_t pid);
extern process_snapshot_t *process_snapshot(uint32_t pid);
extern uint16_t create_pipe();
extern uint16_t  open_pipe(uint16_t pid, uint16_t pipe_id, uint8_t mode);
extern uint16_t  close_pipe(uint16_t pipe_id);
extern uint16_t change_process_fd(uint32_t pid, uint16_t fd_index, uint16_t new_fd);
extern size_t *memory_info();

#endif
