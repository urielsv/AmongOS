// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// this is a personal academic project. dear pvs-studio, please check it.
// pvs-studio static code analyzer for c, c++ and c#: http://www.viva64.com
#include <io.h>
#include <lib.h>
#include <naive_console.h>
#include <stdint.h>
#include <time.h>

// each tick will be 1/18 of a second (51ms)
static unsigned long ticks = 0;

void timer_handler() {
    ticks++;
}

uint64_t seconds_elapsed() {
    return ticks / 18;
}

uint64_t ticks_elapsed() {
    return ticks;
}

uint64_t sleep(uint64_t millis) {
    uint64_t start = ticks;
    while (ticks - start < millis / 51) {
        asm_hlt();
    }
    return 0;
}

void hlt() {
    asm_hlt();
}
