// this is a personal academic project. dear pvs-studio, please check it.
// pvs-studio static code analyzer for c, c++ and c#: http://www.viva64.com
#include <naive_console.h>
#include <stdint.h>
#include "../include/lib.h"
#include <io.h>

#define timezone -3 // argentina

static unsigned int rand_seed = 12345; 


static uint32_t format(uint8_t num) {
    int dec = (num & 240) >> 4;
    int units = num & 15;
    return dec * 10 + units;
}

uint32_t get_seconds() {
    return format(rtc_time(0x00));
}
uint32_t get_minutes() {
    return format(rtc_time(0x02));
}
uint32_t get_hours() {
    return format(rtc_time(0x04)) + timezone;
}

void get_time() {
    nc_print_dec(get_hours());
    nc_print(":");
    nc_print_dec(get_minutes());
    nc_print(":");
    nc_print_dec(get_seconds());
}


uint64_t get_random_number(int seed) {

    rand_seed = rand_seed * 1103515245 + 12345;
    return (unsigned int)(rand_seed / 65536) % 32768;
}

void int_to_string(int num, char *str) {
    int i = 0;
    int is_negative = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    while (num != 0) {
        str[i++] = (num % 10) + '0'; 
        num /= 10;
    }

    if (is_negative) {
        str[i++] = '-';
    }

    str[i] = '\0'; 

    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}

void print_number(int number) {
    char buffer[20];
    int_to_string(number, buffer); 
    ker_write(buffer); 
}


void pointer_to_string(void *ptr, char *buffer, size_t buffer_size) {
    
    memset(buffer, 0, buffer_size);
    
    if (buffer_size < 20) {
        return; 
    }

    uintptr_t address = (uintptr_t)ptr;

    int index = 0;
    buffer[index++] = '0';   
    buffer[index++] = 'x';   

    for (int i = 15; i >= 0; i--) {
        int digit = (address >> (i * 4)) & 0xf; 
        if (digit < 10) {
            buffer[index++] = '0' + digit; 
        } else {
            buffer[index++] = 'a' + (digit - 10); 
        }
    }

    buffer[index] = '\0';
}