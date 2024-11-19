// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <math.h>
#include <stdint.h>

uint64_t abs(int64_t n) {
    return n < 0 ? -n : n;
}