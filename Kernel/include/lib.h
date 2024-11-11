/******************************************************************************
 *
 * @file    lib.h
 *
 * @brief   General purpose functions for the kernel.
 *
 * @author  Luca Pugliese                           <lpugliese@itba.edu.ar>
 * @author  Felipes Venturino                        <fventurino@itba.edu.ar>
 * @author  Uriel Sosa Vazquez                      <usosavazquez@itba.edu.ar>
 *
 ******************************************************************************/
#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <io.h>


extern char *cpuVendor(char *result);


extern uint8_t rtcTime(uint8_t option);

uint32_t getSeconds();
uint32_t getMinutes();
uint32_t getHours();

// Returns string with the number passed as parameter.
// void itoa(char* str, int num, int width);

// Returns string with time format.
void getTime();

//Random
uint64_t get_random_number(int seed);

void int_to_string(int num, char *str);

void print_number(int number);

// Equal to assinging "out val, port" in assembly
extern void outb(unsigned short port, unsigned char val);
// Equal to assinging "in val, port" in assembly
extern uint8_t inb(unsigned short port);

#endif