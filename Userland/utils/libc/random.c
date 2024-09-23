// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <random.h>
#include <syscalls.h>

uint64_t rand(int seed, uint64_t max) {
    return random(seed) % max;
}