// this is a personal academic project. dear pvs-studio, please check it.
// pvs-studio static code analyzer for c, c++ and c#: http://www.viva64.com
#include <random.h>
#include <syscalls.h>

uint64_t rand(int seed, uint64_t max) {
    return random(seed) % max;
}