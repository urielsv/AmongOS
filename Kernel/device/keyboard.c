// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// // #include <font.h>
// // #include <io.h>
// #include <keyboard.h>
// #include <scheduler.h>

// #define buffer_size 128
// #define ctrl_mask 0x1f  // for detecting control key combinations
// #define ctrl_c 0x03     // ascii value for ctrl+c
// #define ctrl_d 0x04     // ascii value for ctrl+d

// static uint8_t shift = 0;
// static uint8_t ctrl = 0;   // new flag for control key
// static uint8_t last_key = 0;
// static char buffer[buffer_size] = {0};
// static int8_t first_ptr = 0;
// static int8_t last_ptr = 0;
// static uint8_t count = 0;

// // function declarations
// static void clear_buffer(void);
// static void handle_control_sequence(uint8_t key);

// void keyboard_handler() {
//     get_key();
// }

// uint8_t get_key() {
//     uint8_t key = asm_get_key();
    
//     // handle modifier keys
//     switch(key) {
//         case lshift:
//         case rshift:
//             shift = 1;
//             return 0;
//         case release_lshift:
//         case release_rshift:
//             shift = 0;
//             return 0;
//         case lctrl:
//             ctrl = 1;
//             return 0;
//         case release_lctrl:
//             ctrl = 0;
//             return 0;
//     }

//     // process regular keys
//     if (key >= 0 && key <= kbd_length) {
//         key = kbd_codes[key][shift];
        
//         // handle control sequences
//         if (ctrl) {
//             handle_control_sequence(key);
//             return key;
//         }
        
//         add_to_buffer(key);
//         last_key = key;
//         return key;
//     }
    
//     return 0;
// }

// static void handle_control_sequence(uint8_t key) {
//     // convert to uppercase for consistent handling
//     uint8_t upper_key = (key >= 'a' && key <= 'z') ? (key - 32) : key;
    
//     // apply ctrl mask and handle special cases
//     uint8_t ctrl_key = upper_key & ctrl_mask;
    
//     switch(ctrl_key) {
//         case ctrl_c:
//             clear_buffer();
//             kill_fg_process();
//             add_to_buffer('^');
//             add_to_buffer('c');
//             add_to_buffer('\n');
//             yield();
//             break;
            
//         case ctrl_d:
//             // handle EOF
//             if (count == 0) {  
//                 // signal_handler(EOF);
//                 add_to_buffer('^');
//                 add_to_buffer('d');
//                 add_to_buffer('\n');
//             }
//             break;
//     }
// }

// void add_to_buffer(uint8_t key) {
//     if (count >= buffer_size) {
//         return;  // buffer full
//     }
    
//     count++;
//     if (key == '\b') {
//         if (count >= 2) count -= 2;
//         else count = 0;
//     }
//     if (key == '\n') {
//         clear_buffer();
//     }
    
//     buffer[last_ptr] = key;
//     last_ptr = (last_ptr + 1) % buffer_size;
// }

// uint64_t get_buffer(char *buff, uint64_t max_count) {
//     if (!buff || max_count == 0) return 0;
    
//     uint64_t i = 0;
//     while (i < max_count && i < count && buff[i] != '\n') {
//         uint8_t current = buffer[first_ptr];
//         // if (current == 0){
//         //     buff[i] = EOF;
//         //     // hlt();
//         //     return i;
//         // }
//         if (current != '\b') {
//             buff[i++] = current;
//         } else if (i > 0) {
//             i--;  // handle backspace
//         }
//         first_ptr = (first_ptr + 1) % buffer_size;
//     }
    
//     first_ptr = 0;
//     return i;
// }

// uint8_t get_last_input() {
//     if (last_key != 0) {
//         last_ptr = (last_ptr - 1 + buffer_size) % buffer_size;
//         uint8_t to_ret = last_key;
//         last_key = 0;
//         return to_ret;
//     }
//     return last_key;
// }

// static void clear_buffer() {
//     for (int i = 0; i < buffer_size; i++) {
//         buffer[i] = 0;
//     }
//     count = 0;
//     last_key = 0;
//     first_ptr = 0;
//     last_ptr = 0;
// }


// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <keyboard.h>
#include <lib.h>
#include <memman.h>
#include <scheduler.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <video.h>

#define BUFFER_CAPACITY 64 /* Longitud maxima del vector _buffer */
#define LCTRL 29
#define LSHIFT 42
#define C_HEX 0x2E
#define D_HEX 0x20
#define R_HEX 0x13
#define M_HEX 0x32
#define RELEASED 0x80 /* Mascara para detectar si se solto una tecla */
#define SHIFTED 0x80
static uint8_t _bufferStart = 0;			  /* Indice del comienzo de la cola */
static uint8_t _bufferSize = 0;				  /* Longitud de la cola */
static int8_t _buffer[BUFFER_CAPACITY] = {0}; /* Vector ciclico que guarda las teclas
											   * que se van leyendo del teclado */
static uint8_t _ctrl = 0;					  /* Flag para detectar si se presiono ctrl */
static uint8_t _shift = 0;					  /* Flag para detectar si se presiono shift */
static const char charHexMap[] =			  /* Mapa de scancode a ASCII */
	{0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',
	 '=', '\b', ' ', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
	 '[', ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
	 ';', '\'', 0, 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',',
	 '.', '/', 0, '*', 0, ' '};
static const char charHexMapShift[] = /* Mapa de scancode con shift a ASCII */
	{0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_',
	 '+', '\b', ' ', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
	 '{', '}', '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
	 ';', '"', 0, 0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<',
	 '>', '?', 0, '*', 0, ' '};

static void writeKey(int8_t key);

static int getBufferIndex(int offset) {
	return (_bufferStart + offset) % (BUFFER_CAPACITY);
}

void initializeKeyboardDriver() {
    sem_open(1, 0);
}

void keyboardHandler() {
	uint8_t key = asm_get_key();
	if (!(key & RELEASED)) {
		if (key == LCTRL)
			_ctrl = 1;
		else if (key == LSHIFT)
			_shift = 1;
		else if (_ctrl) {
			if (key == C_HEX) {
				_bufferStart = _bufferSize = 0;
				kill_fg_process();
			}
			else if (key == D_HEX && _bufferSize < BUFFER_CAPACITY - 1)
				writeKey(EOF);
		}
		else if (_bufferSize < BUFFER_CAPACITY - 1) {
			if (_shift)
				key = SHIFTED | key;
			writeKey(key);
		}
	}
	else {
		if (key == (LCTRL | RELEASED))
			_ctrl = 0;
		else if (key == (LSHIFT | RELEASED))
			_shift = 0;
	}
}

static void writeKey(int8_t key) {
	if (((key & 0x7F) < sizeof(charHexMap) && charHexMap[key & 0x7F] != 0) || (int) key == EOF) {
		_buffer[getBufferIndex(_bufferSize)] = key;
		_bufferSize++;
		sem_post(1);
	}
}

int8_t getScancode() {
	if (_bufferSize > 0) {
		int8_t c = _buffer[getBufferIndex(0)];
		_bufferStart = getBufferIndex(1);
		_bufferSize--;
		return c;
	}
	return 0;
}

int8_t getAscii() {
	sem_wait(1);
	int scanCode = getScancode();
	if (scanCode == EOF)
		return EOF;
	if (SHIFTED & scanCode) {
		scanCode &= 0x7F;
		return charHexMapShift[scanCode];
	}
	return charHexMap[scanCode];
}
