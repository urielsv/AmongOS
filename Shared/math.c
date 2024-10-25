#include <math.h>
#include <stdint.h>

uint64_t abs(int64_t n) {
    return n < 0 ? -n : n;
}