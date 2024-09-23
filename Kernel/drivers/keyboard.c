// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <font.h>
#include <io.h>
#include <keyboard.h>

#define BUFFER_SIZE 128

static uint8_t shift = 0;

static uint8_t last_key = 0;

static char buffer[BUFFER_SIZE] = {0};
static int8_t first_ptr = 0; // Points to the front of the buffer.
static int8_t last_ptr = 0; // Points to the last element added.
static uint8_t count = 0;

static void clear_buffer();

void keyboard_handler() {
    get_key();
}

// Get the ascii value of the key pressed and detect if the shift was also pressed.
uint8_t get_key() {
    uint8_t key = asm_get_key();
    if (key == LSHIFT || key == RSHIFT) {
        shift = 1;
    } else if (key == RELEASE_LSHIFT || key == RELEASE_RSHIFT) {
        shift = 0;
    }

    if (key >= 0 && key <= KBD_LENGTH) {
        key = kbd_codes[key][shift];
        add_to_buffer(key);
        last_key = key;
        return key;
    }

    return 0;
}

void add_to_buffer(uint8_t key) {

    count++;

    if (key == '\b')
        count -= 2;

    if (key == '\n')
        clear_buffer();

    buffer[last_ptr++ % BUFFER_SIZE] = key;
}

uint64_t get_buffer(char *buff, uint64_t count) {
    uint64_t i = 0;
    while (i < count && buff[i] != '\n') {
        if (buffer[first_ptr++ % BUFFER_SIZE] != '\b')
            buff[i++] = buffer[first_ptr % BUFFER_SIZE];
    }

    first_ptr = 0;

    return i;
}

// Get the last input from the buffer and remove it from the buffer.
uint8_t get_last_input() {

    if (last_key != 0) {
        last_ptr = (last_ptr - 1) % BUFFER_SIZE;
        uint8_t to_ret = last_key;
        last_key = 0;
        return to_ret;
    }
    return last_key;
}

static void clear_buffer() {
    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }
    count = 0;
    last_key = 0;
}
