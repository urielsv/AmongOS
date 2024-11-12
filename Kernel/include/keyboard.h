/******************************************************************************
 *
 * @file    keyboard.h
 *
 * @brief   keyboard functions to read from the keyboard.
 *
 * @author  luca pugliese                           <lpugliese@itba.edu.ar>
 * @author  felipes venturino                        <fventurino@itba.edu.ar>
 * @author  uriel sosa vazquez                      <usosavazquez@itba.edu.ar>
 *
 ******************************************************************************/
#ifndef keyboard_h
#define keyboard_h

#include <stdint.h>
#include <naive_console.h>

// special keys
#define lshift 0x2a
#define rshift 0x36
#define release_lshift lshift + 128
#define release_rshift rshift + 128
#define lctrl 0x1d
#define release_lctrl lctrl + 128
#define capslock 0x3a
#define delete 0x0e
#define esc 0x01
#define CTRL 0x1d
#define alt 0x38
#define f1 0x3b
#define f2 0x3c
#define f3 0x3d

#define kbd_length 57

// keyboard codes
static const uint8_t kbd_codes[][2] = {
    {0, 0},
    {0, 0},
    {'1', '!'},
    {'2', '@'},
    {'3', '#'},
    {'4', '$'},
    {'5', '%'},
    {'6', '^'},
    {'7', '&'},
    {'8', '*'},
    {'9', '('},
    {'0', ')'},
    {'-', '_'},
    {'=', '+'},
    {'\b', '\b'},
    {'\t', '\t'},
    {'q', 'q'},
    {'w', 'w'},
    {'e', 'e'},
    {'r', 'r'},
    {'t', 't'},
    {'y', 'y'},
    {'u', 'u'},
    {'i', 'i'},
    {'o', 'o'},
    {'p', 'p'},
    {'[', '{'},
    {']', '}'},
    {'\n', '\n'},
    {0, 0},
    {'a', 'a'},
    {'s', 's'},
    {'d', 'd'},
    {'f', 'f'},
    {'g', 'g'},
    {'h', 'h'},
    {'j', 'j'},
    {'k', 'k'},
    {'l', 'l'},
    {';', ':'},
    {'\'', '\"'},
    {'`', '~'},
    {0, 0},
    {'\'', '|'},
    {'z', 'z'},
    {'x', 'x'},
    {'c', 'c'},
    {'v', 'v'},
    {'b', 'b'},
    {'n', 'n'},
    {'m', 'm'},
    {',', '<'},
    {'.', '>'},
    {'/', '?'},
    {0, 0},
    {0, 0},
    {0, 0},
    {' ', ' '},
};

// get the key value pressed (or released)
extern uint8_t asm_get_key();
// get the last input and manage the buffer
uint8_t get_key();

// wrapper function to get keyboard input (as interrupt)
void keyboard_handler();

// add a key to the buffer
void add_to_buffer(uint8_t key);

// get the last input from the buffer
uint8_t get_last_input();

// get the buffer
uint64_t get_buffer(char *buff, uint64_t count);

// get the first input from the buffer @deprecated
uint8_t get_first_input();

// get the buffer size, used for removing keys until the buffer is empty
uint32_t kbd_count();

#endif