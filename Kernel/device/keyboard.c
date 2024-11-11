#include <font.h>
#include <io.h>
#include <keyboard.h>
#include <scheduler.h>

#define BUFFER_SIZE 128
#define CTRL_MASK 0x1F  // For detecting Control key combinations
#define CTRL_C 0x03     // ASCII value for Ctrl+C
#define CTRL_D 0x04     // ASCII value for Ctrl+D

static uint8_t shift = 0;
static uint8_t ctrl = 0;   // New flag for Control key
static uint8_t last_key = 0;
static char buffer[BUFFER_SIZE] = {0};
static int8_t first_ptr = 0;
static int8_t last_ptr = 0;
static uint8_t count = 0;

// Function declarations
static void clear_buffer(void);
static void handle_control_sequence(uint8_t key);

void keyboard_handler() {
    get_key();
}

uint8_t get_key() {
    uint8_t key = asm_get_key();
    
    // Handle modifier keys
    switch(key) {
        case LSHIFT:
        case RSHIFT:
            shift = 1;
            return 0;
        case RELEASE_LSHIFT:
        case RELEASE_RSHIFT:
            shift = 0;
            return 0;
        case LCTRL:
            ctrl = 1;
            return 0;
        case RELEASE_LCTRL:
            ctrl = 0;
            return 0;
    }

    // Process regular keys
    if (key >= 0 && key <= KBD_LENGTH) {
        key = kbd_codes[key][shift];
        
        // Handle control sequences
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
    // Convert to uppercase for consistent handling
    uint8_t upper_key = (key >= 'a' && key <= 'z') ? (key - 32) : key;
    
    // Apply CTRL mask and handle special cases
    uint8_t ctrl_key = upper_key & CTRL_MASK;
    
    switch(ctrl_key) {
        case CTRL_C:
            clear_buffer();
            kill_fg_process();
            add_to_buffer('^');
            add_to_buffer('C');
            add_to_buffer('\n');
            yield();
            break;
            
        case CTRL_D:
            // Handle EOF
            if (count == 0) {  
                // signal_handler(EOF);
                add_to_buffer('^');
                add_to_buffer('D');
                add_to_buffer('\n');
            }
            break;
    }
}

void add_to_buffer(uint8_t key) {
    if (count >= BUFFER_SIZE) {
        return;  // Buffer full
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
    last_ptr = (last_ptr + 1) % BUFFER_SIZE;
}

uint64_t get_buffer(char *buff, uint64_t max_count) {
    if (!buff || max_count == 0) return 0;
    
    uint64_t i = 0;
    while (i < max_count && i < count && buff[i] != '\n') {
        uint8_t current = buffer[first_ptr];
        if (current != '\b') {
            buff[i++] = current;
        } else if (i > 0) {
            i--;  // Handle backspace
        }
        first_ptr = (first_ptr + 1) % BUFFER_SIZE;
    }
    
    first_ptr = 0;
    return i;
}

uint8_t get_last_input() {
    if (last_key != 0) {
        last_ptr = (last_ptr - 1 + BUFFER_SIZE) % BUFFER_SIZE;
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
    first_ptr = 0;
    last_ptr = 0;
}