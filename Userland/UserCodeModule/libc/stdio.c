// this is a personal academic project. dear pvs-studio, please check it.
// pvs-studio static code analyzer for c, c++ and c#: http://www.viva64.com
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

#include <colors.h>
#include <syscalls.h>
#include <string.h>

void putchar_color(char c, uint64_t fgcolor, uint64_t bgcolor) {
    write(STDOUT, &c, 1, fgcolor, bgcolor);
}

void putchar(char c) {
    putchar_color(c, 0xffffff, 0x000000);
}


int64_t getchar() {
    return read_char();
}

int64_t gets(char *buff, uint64_t length) {
    uint64_t i = 0;
    char c;
    do {
        c = getchar();
        if (c >= 0x20 && c <= 0x7f) {
            buff[i] = c;
            putchar(buff[i++]);
        }

        if (c == '\n') {
            putchar('\n');
            buff[i] = '\0';
        }
        if (c == '\b') {
            if (i > 0) {
                i--;
                putchar('\b');
            }
        }
        if (c == EOF) {
            return i;
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


uint64_t printf_color(const char *fmt, uint32_t fgcolor, uint32_t bgcolor, ...) {
    if (!fmt) return 0;
    
    va_list args;
    va_start(args, bgcolor);
    uint64_t chars_printed = 0;
    static char buffer[1024];
    
    for (uint64_t i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] != '%') {
            putchar_color(fmt[i], fgcolor, bgcolor);
            chars_printed++;
            continue;
        }

        // handle % format specifier
        i++;  // skip the %
        if (fmt[i] == '\0') break;

        memset(buffer, 0, sizeof(buffer));  // clear buffer before each conversion

        switch (fmt[i]) {
            case 'd': {
                int val = va_arg(args, int);
                itoa(val, buffer);
                if (buffer[0]) {  // check if conversion succeeded
                    for (char* p = buffer; *p; p++) {
                        putchar_color(*p, fgcolor, bgcolor);
                        chars_printed++;
                    }
                }
                break;
            }
            case 's': {
                const char* str = va_arg(args, const char*);
                if (str) {
                    for (const char* p = str; *p; p++) {
                        putchar_color(*p, fgcolor, bgcolor);
                        chars_printed++;
                    }
                }
                break;
            }
            case '%':
                putchar_color('%', fgcolor, bgcolor);
                chars_printed++;
                break;
            default:
                // invalid format specifier - print it as is
                putchar_color('%', fgcolor, bgcolor);
                putchar_color(fmt[i], fgcolor, bgcolor);
                chars_printed += 2;
                break;
        }
    }
    
    va_end(args);
    return chars_printed;
}

