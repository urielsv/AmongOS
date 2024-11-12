// this is a personal academic project. dear pvs-studio, please check it.
// pvs-studio static code analyzer for c, c++ and c#: http://www.viva64.com
#include "sound.h"
#include "lib.h"
#include "time.h"
#include <stdint.h>

// play sound using built in speaker
static void play_sound(uint32_t n_frequence) {
    uint32_t div;
    uint8_t tmp;

    // set the pit to the desired frequency
    div = 1193180 / n_frequence;
    outb(0x43, 0xb6);
    outb(0x42, (uint8_t)(div));
    outb(0x42, (uint8_t)(div >> 8));

    // and play the sound using the pc speaker
    tmp = inb(0x61);
    if (tmp != (tmp | 3)) {
        outb(0x61, tmp | 3);
    }
}

// make it shutup
static void nosound() {
    uint8_t tmp = inb(0x61) & 0xfc;

    outb(0x61, tmp);
}

// make a beep
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