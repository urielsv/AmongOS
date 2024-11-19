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

#define BUFFER_CAPACITY 128
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

static void write_key(int8_t key);

static int get_buffer_index(int offset) {
	return (_bufferStart + offset) % (BUFFER_CAPACITY);
}

void initialize_keyboard_driver() {
    sem_open(30, 0);
}

void keyboard_handler() {
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
				write_key(EOF);
		}
		else if (_bufferSize < BUFFER_CAPACITY - 1) {
			if (_shift)
				key = SHIFTED | key;
			write_key(key);
		}
	}
	else {
		if (key == (LCTRL | RELEASED))
			_ctrl = 0;
		else if (key == (LSHIFT | RELEASED))
			_shift = 0;
	}
}

static void write_key(int8_t key) {
	if (((key & 0x7F) < sizeof(charHexMap) && charHexMap[key & 0x7F] != 0) || (int) key == EOF) {
		_buffer[get_buffer_index(_bufferSize)] = key;
		_bufferSize++;
		sem_post(1);
	}
}

int8_t get_scancode() {
	if (_bufferSize > 0) {
		int8_t c = _buffer[get_buffer_index(0)];
		_bufferStart = get_buffer_index(1);
		_bufferSize--;
		return c;
	}
	return 0;
}

int8_t get_ascii() {
	sem_wait(1);
	int scanCode = get_scancode();
	if (scanCode == EOF)
		return EOF;
	if (SHIFTED & scanCode) {
		scanCode &= 0x7F;
		return charHexMapShift[scanCode];
	}
	return charHexMap[scanCode];
}
