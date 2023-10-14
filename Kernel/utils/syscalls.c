#include <naiveConsole.h>
#include "syscalls.h"
#include "keyboard.h"
#include "time.h"
#include "io.h"


// temp
#include <naiveConsole.h>

// Cast to a function pointer
typedef uint64_t (*syscall_t)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

// Array of function pointers
static syscall_t syscalls[] = {
    (syscall_t)&sys_read,   // sys_id 0
    (syscall_t)&sys_write, // sys_id 1
    (syscall_t)&pid,
    (syscall_t)&sys_exec,
    (syscall_t)&sys_ticks,
    (syscall_t)&sys_seconds,
};

uint64_t syscall_dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {


    syscall_t syscall = syscalls[r9];
    if (syscall != 0) {
        return syscall(rdi, rsi, rdx, rcx, r8);
    }
    return 0;
}

uint64_t sys_write(uint8_t fd, const char *buffer, uint64_t count)
{
    if (buffer == 0) {
        return 0;
    }

    // @TODO: Check if fd is valid and replace magic numbers
    // stdout
    if (fd == 1) {
        return printf(buffer);
    }

    // stderr
    else if (fd == 2)
        return printf_color(buffer, 0xFF, 0x40);

    return 0;
}

char sys_read(){
    char c = get_last_input();
    putchar(c);
    return c;
}


uint64_t pid() {
    return 0;
}
void sys_exec(){

}

uint64_t sys_ticks(){
    return ticks_elapsed();
}

uint64_t sys_seconds(){
    return seconds_elapsed();
}