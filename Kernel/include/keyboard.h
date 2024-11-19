#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_
#include <stdint.h>
/* Handler de interrupciones de teclado */
void keyboard_handler();

/* Devuelve el valor ascii del ultimo caracter en el buffer de teclado */
int8_t get_ascii();

/* Devuelve el scancode del ultimo caracter en el buffer de teclado */
int8_t get_scancode();

void initialize_keyboard_driver();

extern uint8_t asm_get_key();


#endif
