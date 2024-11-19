// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "io.h"
#include "video.h"
#include <definitions.h>

// starting position
static uint64_t x = 0;
static uint64_t y = 0;

void clear(uint32_t hex) {
    clear_screen(hex);
    x = 0;
    y = 0;
}

void set_position(uint64_t new_x, uint64_t new_y) {
        x = new_x;
        y = new_y;
}

// 8 x 16 pixels
uint64_t ker_write(const char *str, ...) {
    return ker_write_color(str, 0xffffff, 0x000000);
}

uint64_t ker_write_color(const char *str, uint64_t fgcolor, uint64_t bgcolor) {
    int i = 0;
  //  uint64_t tempx, tempy;
    while (str[i]) {
        putchar_color_k(str[i], fgcolor, bgcolor);
        i++;
    }
    return i;
}

void putchar_color_k(char c, uint64_t fgcolor, uint64_t bgcolor) {
    switch (c) {
    case '\n':
        new_line(&x, &y);
        break;
    case '\b':
        delete_char(&x, &y, fgcolor, bgcolor);
        break;
    case '\t':
        for (int i = 0; i < 4; i++)
           put_char_at(' ', &x, &y, fgcolor, bgcolor);
        break;
    case EOF:
        // handling of eof
        put_char_at('\n', &x, &y, fgcolor, bgcolor);
        break;
    default:
        if (c >= 0x20)// && c <= 0x7f)
            put_char_at(c, &x, &y, fgcolor, bgcolor);
        break;
    }
}

void putchar_k(char c) {
    putchar_color_k(c, 0xffffff, 0x000000);
}

