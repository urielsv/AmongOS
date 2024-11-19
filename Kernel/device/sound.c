// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "sound.h"
#include "lib.h"
#include "time.h"
#include <stdint.h>

static void play_sound(uint32_t n_frequence) {
    uint32_t div;
    uint8_t tmp;

    div = 1193180 / n_frequence;
    outb(0x43, 0xb6);
    outb(0x42, (uint8_t)(div));
    outb(0x42, (uint8_t)(div >> 8));

    tmp = inb(0x61);
    if (tmp != (tmp | 3)) {
        outb(0x61, tmp | 3);
    }
}

static void nosound() {
    uint8_t tmp = inb(0x61) & 0xfc;

    outb(0x61, tmp);
}

void beep() {
    play_sound(1000);
    sleep(100);
    nosound();
}

void start_sound(uint32_t n_frequence, uint32_t time) {
    if (time != 0) {
        play_sound(n_frequence);
        sleep(time);
    }
    nosound();
}