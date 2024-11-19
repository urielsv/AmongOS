// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <time.h>
#include <keyboard.h>

enum irq{
    timer = 0, keyboard
};

void irq_dispatcher(uint64_t irq) {
    switch(irq) 
    {
        case timer: {
            timer_handler();
                break;
        } 
        case keyboard: {
            keyboard_handler();
                break;
        }
    }
}