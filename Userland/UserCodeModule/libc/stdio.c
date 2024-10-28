// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

#include <colors.h>
#include <syscalls.h>

void putchar_color(char c, uint64_t fgcolor, uint64_t bgcolor) {
    write(1, &c, 1, fgcolor, bgcolor);
}

void putchar(char c) {
    putchar_color(c, 0xFFFFFF, 0x000000);
}


uint8_t getchar() {
    return read_char();
}

int64_t gets(char *buff, uint64_t length) {
    uint64_t i = 0;
    char c;
    do {
        c = getchar();
        if (c >= 0x20 && c <= 0x7F) {
            buff[i] = c;
            putchar(buff[i++]);
        }

        if (c == '\n') {
            buff[i] = '\0';
        }
        if (c == '\b') {
            if (i > 0) {
                i--;
                putchar('\b');
            }
        }
    } while (i < length && c != '\n');

    return i;
}

void scanf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (uint64_t i = 0; fmt[i]; i++) {
        if (fmt[i] == '%') {
            switch (fmt[++i]) {
            case 'd':;
                // max length of int to be read is 24 (arbitrary)
                char buffer[24];
                gets(buffer, 24);
                int val = atoi(buffer);
                int *aux = va_arg(args, int *);
                *aux = val;
                break;
            case 's':
                gets(va_arg(args, char *), 100);
                break;
            case 'c':
                gets(va_arg(args, char *), 1);
                break;
            default:
                break;
            }
        } else {
            putchar(fmt[i]);
        }
    }
    va_end(args);
}

static void print_str(const char *s, uint32_t fgcolor, uint32_t bgcolor) {

    uint64_t i = 0;
    while (s[i]) {
        putchar_color(s[i++], fgcolor, bgcolor);
    }
}

static void lltoa(int64_t value, char *buffer) {
    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    int is_negative = 0;
    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    int i = 0;
    while (value != 0) {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }

    if (is_negative) {
        buffer[i++] = '-';
    }

    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }

    buffer[i] = '\0';
}

uint64_t printf_color(const char *fmt, uint32_t fgcolor, uint32_t bgcolor, ...) {
    va_list args;
    va_start(args, bgcolor);
    uint64_t i = 0;
    char buffer[32]; // Increased buffer size for 64-bit numbers

    while (fmt[i]) {
        if (fmt[i] == '%') {
            i++; // Move past %
            
            // Check for length modifier
            int is_long_long = 0;
            if (fmt[i] == 'l' && fmt[i + 1] == 'l') {
                is_long_long = 1;
                i += 2; // Skip both 'l's
            }

            switch (fmt[i]) {
                case 'd':
                    if (is_long_long) {
                        lltoa(va_arg(args, int64_t), buffer);
                    } else {
                        itoa(va_arg(args, int), buffer);
                    }
                    print_str(buffer, fgcolor, bgcolor);
                    break;

                case 's':
                    print_str(va_arg(args, char *), fgcolor, bgcolor);
                    break;

                case '%':
                    putchar_color('%', fgcolor, bgcolor);
                    break;

                default:
                    // Handle unknown format specifier
                    putchar_color('%', fgcolor, bgcolor);
                    putchar_color(fmt[i], fgcolor, bgcolor);
                    break;
            }
        } else {
            putchar_color(fmt[i], fgcolor, bgcolor);
        }
        i++;
    }

    va_end(args);
    return i;
}