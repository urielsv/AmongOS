#ifndef naive_console_h
#define naive_console_h

#include <stdint.h>

void nc_backspace();
void nc_print(const char * string);
void nc_print_color(const char * string, const uint8_t fg_color, const uint8_t bg_color);
void nc_print_char(char character);
void nc_print_char_color(char character, uint8_t fg_color, uint8_t bg_color);
void nc_newline();
void nc_print_dec(uint64_t value);
void nc_print_hex(uint64_t value);
void nc_print_bin(uint64_t value);
void nc_print_base(uint64_t value, uint32_t base);
void nc_clear();
void set_bg_color(uint8_t new_bg_color);
void set_fg_color(uint8_t new_fg_color);
uint32_t uint_to_base(uint64_t value, char * buffer, uint32_t base);
#endif