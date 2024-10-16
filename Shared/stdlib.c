// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>

void *memset(void *destination, int32_t c, uint64_t length) {
    uint8_t chr = (uint8_t)c;
    char *dst = (char *)destination;

    while (length--)
        dst[length] = chr;

    return destination;
}

void *memcpy(void *destination, const void *source, uint64_t length) {
    /*
     * memcpy does not support overlapping buffers, so always do it
     * forwards. (Don't change this without adjusting memmove.)
     *
     * For speedy copying, optimize the common case where both pointers
     * and the length are word-aligned, and copy word-at-a-time instead
     * of byte-at-a-time. Otherwise, copy by bytes.
     *
     * The alignment logic below should be portable. We rely on
     * the compiler to be reasonably intelligent about optimizing
     * the divides and modulos out. Fortunately, it is.
     */
    uint64_t i;

    if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
        (uint64_t)source % sizeof(uint32_t) == 0 &&
        length % sizeof(uint32_t) == 0) {
        uint32_t *d = (uint32_t *)destination;
        const uint32_t *s = (const uint32_t *)source;

        for (i = 0; i < length / sizeof(uint32_t); i++)
            d[i] = s[i];
    } else {
        uint8_t *d = (uint8_t *)destination;
        const uint8_t *s = (const uint8_t *)source;

        for (i = 0; i < length; i++)
            d[i] = s[i];
    }

    return destination;
}

static void reverse_str(char str[], int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

uint64_t atoi(char *str) {
    uint64_t ans = 0;

    int i = 0;
    uint8_t sign = 1;

    if (str[i] == '-') {
        sign = -1;
        i++;
    }

    while (str[i] >= '0' && str[i] <= '9') {
        ans = ans * 10 + (str[i] - '0');
        i++;
    }
    return ans * sign;
}

void itoa(uint64_t number, char *buff) {
    int i = 0;
    int is_neg = 0;

    if (number == 0) {
        buff[i++] = '0';
        buff[i] = '\0';
        return;
    }

    // Handle negative numbers
    if (number < 0) {
        is_neg = 1;
        number = -number;
    }

    while (number != 0) {
        int digit = number % 10;
        buff[i++] = '0' + digit;
        number = number / 10;
    }

    if (is_neg) {
        buff[i++] = '-';
    }

    buff[i] = '\0';

    reverse_str(buff, i);
}
