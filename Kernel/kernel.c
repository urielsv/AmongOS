// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <idt_loader.h>
#include <io.h>
#include <keyboard.h>
#include <lib.h>
#include <module_loader.h>
#include <naive_console.h>
#include <sound.h>
#include <stdint.h>
#include <string.h>
#include <syscalls.h>
#include <time.h>
#include <userland.h>
#include <video.h>
#include <exceptions.h>
#include <memman.h>
#include <scheduler.h>
#include <stdlib.h>
#include <pipes.h>

#define string_size "1000000"

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t end_of_kernel_binary;
extern uint8_t end_of_kernel;


static const uint64_t page_size = 0x1000;

void clear_bss(void *bss_address, uint64_t bss_size)
{
    memset(bss_address, 0, bss_size);
}

void *get_stack_base()
{
    return (void *)((uint64_t)&end_of_kernel + page_size * 8 // the size of the stack itself, 32_ki_b
                    - sizeof(uint64_t)                    // begin at the top of the stack
    );
}

void *initialize_kernel_binary()
{

    void *module_addresses[] = {
        userland_code_module_address,
        sample_data_module_address};

    load_modules(&end_of_kernel_binary, module_addresses);

    clear_bss(&bss, &end_of_kernel - &bss);

    uint64_t size = (uint64_t)heap_end_address - (uint64_t)heap_start_address;
    b_init(heap_start_address, size);
    init_scheduler();
    init_pipe_manager();
    initialize_keyboard_driver();
    return get_stack_base();
}

int main()
{

    create_process((function)userland_code_module_address, NULL, 0, "shell", 4, 1);

    idt_loader();
    
    while(1) ;

    return 0;
}
