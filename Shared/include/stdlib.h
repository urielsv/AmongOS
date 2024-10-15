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

#define NULL ((void*)0)


// Integer to Array
void itoa(uint64_t number, char* buff);

// Array to integer
uint64_t atoi(char* str);

#endif