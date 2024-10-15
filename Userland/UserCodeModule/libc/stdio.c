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
                gets(va_arg(args, char), 1);
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

uint64_t printf_color(const char *fmt, uint32_t fgcolor, uint32_t bgcolor, ...) {
    va_list args;
    va_start(args, fmt);

    uint64_t i = 0;
    while (fmt[i]) {
        if (fmt[i] == '%') {
            switch (fmt[++i]) {
            case 'd':;
                char buff[20];
                itoa(va_arg(args, int), buff);
                print_str(buff, fgcolor, bgcolor);
                break;
            case 's':
                print_str(va_arg(args, char *), fgcolor, bgcolor);
                break;

            case '%':
                putchar_color('%', fgcolor, bgcolor);
                break;
            default:
                break;
            }
        } else {
            putchar_color(fmt[i], fgcolor, bgcolor);
        }
        i++;
        va_end(args);
    }
    return i;
}
