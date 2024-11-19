// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

void itoa(int64_t num, char* str) {
    int i = 0;
    int isNegative = 0;
    
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    
    // Handle negative numbers
    if (num < 0) {
        isNegative = 1;
        num = -num;  
    }
    
    while (num != 0) {
        int64_t rem = num % 10;
        str[i++] = rem + '0';
        num = num / 10;
    }
    
    if (isNegative)
        str[i++] = '-';
    
    str[i] = '\0';  
    
    reverse_str(str, i);
}


int64_t atoi(const char* str) {
    if (str == NULL) {
        return 0;
    }
    
    // Skip leading whitespace
    while (*str == ' ' || *str == '\t') {
        str++;
    }
    
    // Handle sign
    int64_t sign = 1;
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    // Process digits
    int64_t result = 0;
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return sign * result;
}