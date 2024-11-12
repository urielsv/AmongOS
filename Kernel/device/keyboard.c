#include <font.h>
#include <io.h>
#include <keyboard.h>
#include <scheduler.h>

#define buffer_size 128
#define ctrl_mask 0x1f  // for detecting control key combinations
#define ctrl_c 0x03     // ascii value for ctrl+c
#define ctrl_d 0x04     // ascii value for ctrl+d

static uint8_t shift = 0;
static uint8_t ctrl = 0;   // new flag for control key
static uint8_t last_key = 0;
static char buffer[buffer_size] = {0};
static int8_t first_ptr = 0;
static int8_t last_ptr = 0;
static uint8_t count = 0;

// function declarations
static void clear_buffer(void);
static void handle_control_sequence(uint8_t key);

void keyboard_handler() {
    get_key();
}

uint8_t get_key() {
    uint8_t key = asm_get_key();
    
    // handle modifier keys
    switch(key) {
        case lshift:
        case rshift:
            shift = 1;
            return 0;
        case release_lshift:
        case release_rshift:
            shift = 0;
            return 0;
        case lctrl:
            ctrl = 1;
            return 0;
        case release_lctrl:
            ctrl = 0;
            return 0;
    }

    // process regular keys
    if (key >= 0 && key <= kbd_length) {
        key = kbd_codes[key][shift];
        
        // handle control sequences
        if (ctrl) {
            handle_control_sequence(key);
            return key;
        }
        
        add_to_buffer(key);
        last_key = key;
        return key;
    }
    
    return 0;
}

static void handle_control_sequence(uint8_t key) {
    // convert to uppercase for consistent handling
    uint8_t upper_key = (key >= 'a' && key <= 'z') ? (key - 32) : key;
    
    // apply ctrl mask and handle special cases
    uint8_t ctrl_key = upper_key & ctrl_mask;
    
    switch(ctrl_key) {
        case ctrl_c:
            clear_buffer();
            kill_fg_process();
            add_to_buffer('^');
            add_to_buffer('c');
            add_to_buffer('\n');
            yield();
            break;
            
        case ctrl_d:
            // handle eof
            if (count == 0) {  
                // signal_handler(eof);
                add_to_buffer('^');
                add_to_buffer('d');
                add_to_buffer('\n');
            }
            break;
    }
}

void add_to_buffer(uint8_t key) {
    if (count >= buffer_size) {
        return;  // buffer full
    }
    
    count++;
    if (key == '\b') {
        if (count >= 2) count -= 2;
        else count = 0;
    }
    if (key == '\n') {
        clear_buffer();
    }
    
    buffer[last_ptr] = key;
    last_ptr = (last_ptr + 1) % buffer_size;
}

uint64_t get_buffer(char *buff, uint64_t max_count) {
    if (!buff || max_count == 0) return 0;
    
    uint64_t i = 0;
    while (i < max_count && i < count && buff[i] != '\n') {
        uint8_t current = buffer[first_ptr];
        if (current != '\b') {
            buff[i++] = current;
        } else if (i > 0) {
            i--;  // handle backspace
        }
        first_ptr = (first_ptr + 1) % buffer_size;
    }
    
    first_ptr = 0;
    return i;
}

uint8_t get_last_input() {
    if (last_key != 0) {
        last_ptr = (last_ptr - 1 + buffer_size) % buffer_size;
        uint8_t to_ret = last_key;
        last_key = 0;
        return to_ret;
    }
    return last_key;
}

static void clear_buffer() {
    for (int i = 0; i < buffer_size; i++) {
        buffer[i] = 0;
    }
    count = 0;
    last_key = 0;
    first_ptr = 0;
    last_ptr = 0;
}