/******************************************************************************
 *
 * @file    stdlib.h
 *
 * @brief   standard input/output functions. (similar, but primitive, to c stdio.h)
 *
 * @author  luca pugliese                           <lpugliese@itba.edu.ar>
 * @author  felipes venturino                        <fventurino@itba.edu.ar>
 * @author  uriel sosa vazquez                      <usosavazquez@itba.edu.ar>
 *
 ******************************************************************************/
#ifndef stdio_h
#define stdio_h

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <colors.h>

void putchar_color(char c, uint64_t fgcolor, uint64_t bgcolor);
void putchar(char c);

// prints a string to the screen
uint64_t printf_color(const char* format, uint32_t fgcolor, uint32_t bgcolor, ...);

// reads from keyboard a string of length characters, modifying the pointers as parameter.
void scanf(const char* format, ...);



// read from keyboard a string of length characters
int64_t gets(char *buff, uint64_t length);
// read a character
int64_t getchar();

#define printf(fmt, ...) printf_color(fmt, color_white, color_black, ##__VA_ARGS__)


#endif