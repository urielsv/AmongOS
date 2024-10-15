// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>


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
