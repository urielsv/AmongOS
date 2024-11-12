/******************************************************************************
 *
 * @file    lib.h
 *
 * @brief   general purpose functions for the kernel.
 *
 * @author  luca pugliese                           <lpugliese@itba.edu.ar>
 * @author  felipes venturino                        <fventurino@itba.edu.ar>
 * @author  uriel sosa vazquez                      <usosavazquez@itba.edu.ar>
 *
 ******************************************************************************/
#ifndef lib_h
#define lib_h

#include <stdint.h>
#include <stddef.h>


extern char *cpu_vendor(char *result);


extern uint8_t rtc_time(uint8_t option);

uint32_t get_seconds();
uint32_t get_minutes();
uint32_t get_hours();

// returns string with the number passed as parameter.
// void itoa(char* str, int num, int width);

// returns string with time format.
void get_time();

//random
uint64_t get_random_number(int seed);

void int_to_string(int num, char *str);

void print_number(int number);

void pointer_to_string(void *ptr, char *buffer, size_t buffer_size);

// equal to assinging "out val, port" in assembly
extern void outb(unsigned short port, unsigned char val);
// equal to assinging "in val, port" in assembly
extern uint8_t inb(unsigned short port);

#endif