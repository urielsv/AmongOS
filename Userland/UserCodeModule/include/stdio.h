/******************************************************************************
 *
 * @file    stdlib.h
 *
 * @brief   Standard Input/Output functions. (Similar, but primitive, to C stdio.h)
 *
 * @author  Luca Pugliese                           <lpugliese@itba.edu.ar>
 * @author  Felipes Venturino                        <fventurino@itba.edu.ar>
 * @author  Uriel Sosa Vazquez                      <usosavazquez@itba.edu.ar>
 *
 ******************************************************************************/
#ifndef STDIO_H
#define STDIO_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <colors.h>

void putchar_color(char c, uint64_t fgcolor, uint64_t bgcolor);
void putchar(char c);

// Prints a string to the screen
uint64_t printf_color(const char* format, uint32_t fgcolor, uint32_t bgcolor, ...);

// Reads from keyboard a string of length characters, modifying the pointers as parameter.
void scanf(const char* format, ...);



// Read from keyboard a string of length characters
int64_t gets(char *buff, uint64_t length);
// Read a character
uint8_t getchar();

#define printf(fmt, ...) printf_color(fmt, COLOR_WHITE, COLOR_BLACK, ##__VA_ARGS__)


#endif