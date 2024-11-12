// this is a personal academic project. dear pvs-studio, please check it.
// pvs-studio static code analyzer for c, c++ and c#: http://www.viva64.com
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