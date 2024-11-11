// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <time.h>
#include <keyboard.h>

enum irq{
    TIMER = 0, KEYBOARD
};

void irq_dispatcher(uint64_t irq) {
    switch(irq) 
    {
        case TIMER: {
            timer_handler();
                break;
        } 
        case KEYBOARD: {
            keyboard_handler();
                break;
        }
    }
}