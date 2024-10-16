/******************************************************************************
 *
 * @file    stdlib.h
 *
 * @brief   Standard library functions. (Similar, but primitive, to C stdlib.h)
 *
 * @author  Luca Pugliese                           <lpugliese@itba.edu.ar>
 * @author  Felipes Venturino                        <fventurino@itba.edu.ar>
 * @author  Uriel Sosa Vazquez                      <usosavazquez@itba.edu.ar>
 *
 ******************************************************************************/
#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>



// Integer to Array
void itoa(uint64_t number, char* buff);

// Array to integer
uint64_t atoi(char* str);
void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

#endif