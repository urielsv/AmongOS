// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "syscalls.h"
#include "io.h"
#include <stdlib.h>
#include "keyboard.h"
#include "sound.h"
#include "time.h"
#include "video.h"
#include <exceptions.h>
#include "lib.h"
#include <naiveConsole.h>
#include <memman.h>
#include <scheduler.h>
#include <process.h>
#include <stdint.h>
#include <stddef.h>
#include <semaphore.h>
#include <pipes.h>
#define REGS_SIZE 19

static uint8_t regs_flag = 0;
static uint64_t regs[REGS_SIZE];

// Cast to a function pointer
typedef uint64_t (*syscall_t)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

// Array of function pointers
static syscall_t syscalls[] = {
    [0] = (syscall_t)&sys_read,
    [1] = (syscall_t)&sys_write,
    [2] = (syscall_t)NULL, // open
    [3] = (syscall_t)NULL, // open
    [4] = (syscall_t)&sys_ticks,
    [5] = (syscall_t)&sys_seconds,
    [6] = (syscall_t)&sys_random_number,
    [7] = (syscall_t)&sys_read_char,
    [8] = (syscall_t)&draw,
    [9] = (syscall_t)&sys_sleep,
    [10] = (syscall_t)&sys_time,
    [11] = (syscall_t)NULL, // open
    [12] = (syscall_t)&sys_hlt,
    [13] = (syscall_t)&sys_clear,
    [14] = (syscall_t)&sys_writing_position,
    [15] = (syscall_t)&screen_info,
    [16] = (syscall_t)&font_size,
    [17] = (syscall_t)&sys_registers,
    [18] = (syscall_t)&test_exc_zero,
    [19] = (syscall_t)&test_exc_invalid_opcode,
    [20] = (syscall_t)&sys_mem_alloc,
    [21] = (syscall_t)&sys_mem_free,
    [22] = (syscall_t)&sys_create_process,
    [23] = (syscall_t)&sys_kill_process,
    [24] = (syscall_t)&sys_block_process,
    [25] = (syscall_t)&sys_unblock_process,
    [26] = (syscall_t)&sys_set_priority,
    [27] = (syscall_t)&sys_get_pid,
    [28] = (syscall_t)&sys_yield,
    [29] = (syscall_t)&sys_sem_open,
    [30] = (syscall_t)&sys_sem_wait,
    [31] = (syscall_t)&sys_sem_post,
    [32] = (syscall_t)&sys_sem_close,
    [33] = (syscall_t)&sys_create_pipe,
    [34] = (syscall_t)&sys_open_pipe,
    [35] = (syscall_t)&sys_close_pipe,

};

uint64_t syscall_dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {

    syscall_t syscall = syscalls[r9];
    if (syscall != 0) {
        return syscall(rdi, rsi, rdx, rcx, r8);
    }
    return 0;
}

uint64_t sys_write(uint8_t fd, const char *buffer, uint64_t count, uint64_t fgcolor, uint64_t bgcolor) {
    if (buffer == NULL || fd < 0) {
        return 0;
    }

    // stdout
    if (fd == 1) {
        return ker_write_color(buffer, fgcolor, bgcolor);
    }

    // stderr
    else if (fd == 2) {
        return ker_write_color(buffer, 0xFF, 0x40);
    }

    //a priori no hay q hacer nada para pipes, pues fds siempre van a ser 0, 1, 2
    else {

    }

    return 0;
}

char *sys_read(uint8_t fd, char *buffer, uint64_t count) {
    get_buffer(buffer, count);

    return 0;
}

char sys_read_char() {
    hlt();
    return get_last_input();
}


uint64_t sys_ticks() {
    return ticks_elapsed();
}

uint64_t sys_seconds() {
    return seconds_elapsed();
}

uint64_t sys_random_number(int seed) {
    return get_random_number(seed);
}

void draw(uint32_t color, uint64_t posx, uint64_t posy) {
    // pixel is 8x8
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            put_pixel(color, posx + j, posy + i);
        }
    }
}

char *sys_time() {
    // in format: hh:mm:ss
    static char time[9];
    uint32_t hours = getHours();
    uint32_t minutes = getMinutes();
    uint32_t seconds = getSeconds();

    char hh[3], mm[3], ss[3];
    itoa(hours, hh);
    itoa(minutes, mm);
    itoa(seconds, ss);

    // Format the time string as "hh:mm:ss"
    time[0] = hh[0];
    time[1] = hh[1];
    time[2] = ':';
    time[3] = mm[0];
    time[4] = mm[1];
    time[5] = ':';
    time[6] = ss[0];
    time[7] = ss[1];
    time[8] = '\0';

    return time;
}

void sys_sleep(uint64_t millis) {
    sleep(millis);
}

void sys_hlt() {
    hlt();
}

void sys_clear(uint32_t color) {
    clear(color);
}

void sys_writing_position(uint64_t x, uint64_t y) {
    set_position(x, y);
}

void screen_info(uint64_t *width, uint64_t *height) {
    *width = get_width();
    *height = get_height();
}

void font_size(int size) {
    video_fontsize(size);
}

void sys_registers() {
    // print_regs(regs_flag ? regs : 0);
    regs_flag = 0;
}

void save_registers(uint64_t *stack) {
    regs_flag = 1;
    for (int i = 0; i < REGS_SIZE; i++){
        regs[i] = stack[i];
    }
}

void test_exc_zero() {
    int a = 0;
    int b = 1 / a;
    (void)b;
}

void test_exc_invalid_opcode() {
    __asm__("ud2");
}

void sys_mem_alloc(size_t size) {
    mem_alloc(size);
}

void sys_mem_free(void *ptr) {
    mem_free(ptr);
}

void sys_create_process(Function code, char **argv, int argc, char *name, uint8_t priority) {
    // No other processes should be unkilleable.
    create_process(code, argv, argc, name, priority, 0);
}

int sys_kill_process(uint64_t pid) {
    return kill_process(pid);
}

int sys_block_process(uint64_t pid) {
    return block_process(pid);
}

int sys_unblock_process(uint64_t pid) {
    return unblock_process(pid);
}

int sys_set_priority(uint64_t pid, uint8_t priority) {
    process_priority(pid, priority);
    return 0;
}

int sys_get_pid() {
    return get_current_pid();
}

void sys_yield() {
    yield();
}

int sys_sem_open(int64_t id, int64_t initialValue) {
    return sem_open(id, initialValue);
}

void sys_sem_wait(int64_t id) {
    sem_wait(id);
}

void sys_sem_post(int64_t id) {
    sem_post(id);
}

void sys_sem_close(int64_t id) {
    sem_close(id);
}

uint32_t sys_waitpid(uint64_t pid, int *status) {
    return waitpid(pid, status);
}

uint16_t sys_create_pipe() {
    return create_pipe();
}

uint16_t sys_open_pipe(uint16_t pipe_id, uint8_t mode) {
    return open_pipe(pipe_id, mode);
}

uint16_t sys_close_pipe(uint16_t pipe_id) {
    return close_pipe(pipe_id);
}