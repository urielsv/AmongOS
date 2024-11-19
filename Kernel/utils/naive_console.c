// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <naive_console.h>

static char buffer[64] = {'0'};
static uint8_t *const video = (uint8_t *)0xb8000;
static uint8_t *current_video = (uint8_t *)0xb8000;
static const uint32_t width = 80;
static const uint32_t height = 25;
static uint8_t default_fg_color = 0xff;
static uint8_t default_bg_color = 0x00;

void nc_print_color(const char *string, const uint8_t fg_color, const uint8_t bg_color) {
    int i;

    for (i = 0; string[i] != 0; i++) {
        if (string[i] == '\n')
            nc_newline();
        else
            nc_print_char_color(string[i], fg_color, bg_color);
    }
}

void nc_backspace() {
    if (current_video >= video + 0x2) {
        current_video -= 2;
        *current_video = ' ';
    }
}

void nc_print(const char *string) {
    nc_print_color(string, default_fg_color, default_bg_color);
}

void set_bg_color(uint8_t new_bg_color) {
    default_bg_color = new_bg_color;
}

void set_fg_color(uint8_t new_fg_color) {
    default_fg_color = new_fg_color;
}

void nc_print_char_color(char character, uint8_t fg_color, uint8_t bg_color) {
    *current_video = character;
    current_video++;
    *current_video = (bg_color & 0xf0) | (fg_color & 0x0f);
    current_video++;
}

void nc_print_char(char character) {
    nc_print_char_color(character, default_fg_color, default_bg_color);
}

void nc_newline() {
    do {
        nc_print_char(' ');
    } while ((uint64_t)(current_video - video) % (width * 2) != 0);
}

void nc_print_dec(uint64_t value) {
    nc_print_base(value, 10);
}

void nc_print_hex(uint64_t value) {
    nc_print_base(value, 16);
}

void nc_print_bin(uint64_t value) {
    nc_print_base(value, 2);
}

void nc_print_base(uint64_t value, uint32_t base) {
    uint_to_base(value, buffer, base);
    nc_print(buffer);
}

void nc_clear() {
    int i;

    for (i = 0; i < height * width; i++) {
        video[i * 2] = ' ';
        video[i * 2 + 1] = default_bg_color;
    }
    current_video = video;
}

uint32_t uint_to_base(uint64_t value, char *buffer, uint32_t base) {
    char *p = buffer;
    char *p1, *p2;
    uint32_t digits = 0;

    // calculate characters for each digit
    do {
        uint32_t remainder = value % base;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
        digits++;
    } while (value /= base);

    // terminate string in buffer.
    *p = 0;

    // reverse string in buffer.
    p1 = buffer;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }

    return digits;
}
