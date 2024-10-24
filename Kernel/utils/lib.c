// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <naiveConsole.h>
#include <stdint.h>
#include "../include/lib.h"

#define TIMEZONE -3 // argentina

static unsigned int randSeed = 12345; // Initial seed for the random number generator


static uint32_t format(uint8_t num) {
    int dec = (num & 240) >> 4;
    int units = num & 15;
    return dec * 10 + units;
}

uint32_t getSeconds() {
    return format(rtcTime(0x00));
}
uint32_t getMinutes() {
    return format(rtcTime(0x02));
}
uint32_t getHours() {
    return format(rtcTime(0x04)) + TIMEZONE;
}

void getTime() {
    ncPrintDec(getHours());
    ncPrint(":");
    ncPrintDec(getMinutes());
    ncPrint(":");
    ncPrintDec(getSeconds());
}

// void itoa(char *str, int num, int width) {
//     for (int i = width - 1; i >= 0; i--) {
//         str[i] = num % 10 + '0';
//         num /= 10;
//     }
//     str[width] = '\0';
// }

uint64_t get_random_number(int seed) {

    randSeed = randSeed * 1103515245 + 12345;
    return (unsigned int)(randSeed / 65536) % 32768;
}