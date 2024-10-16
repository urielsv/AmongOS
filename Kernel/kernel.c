// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <idtLoader.h>
#include <io.h>
// #include <stdio.h>
#include <keyboard.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
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
// #include "./include/scheduler.h"


#define STRING_SIZE "1000000"


extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

//extern void asm_getsp();
//extern void asm_getbp();

static const uint64_t PageSize = 0x1000;

void clearBSS(void *bssAddress, uint64_t bssSize) {
    memset(bssAddress, 0, bssSize);
}

void *getStackBase() {
    return (void *)((uint64_t)&endOfKernel + PageSize * 8 // The size of the stack itself, 32KiB
                    - sizeof(uint64_t) // Begin at the top of the stack
    );
}

void *initializeKernelBinary() {

    void *moduleAddresses[] = {
        userlandCodeModuleAddress,
        sampleDataModuleAddress};

    loadModules(&endOfKernelBinary, moduleAddresses);
    
    clearBSS(&bss, &endOfKernel - &bss);
    
    uint64_t size = (uint64_t) heapEndAddress - (uint64_t) heapStartAddress;
    mem_init(heapStartAddress, size);
    init_scheduler(); 
    return getStackBase();
}


void test(char c){
        ker_write("Test a");
}

int main() {

    char *args_shell[2] = {"shell", NULL};
        ker_write("Contrareciproco");


    // ker_write("Initializing kernel\n");
    // TODO Add Idle process (as process init)
    create_process((Function) userlandCodeModuleAddress, args_shell, 2, "shell", 4, 1);
     char *  test_argsa[1] = {"a"};
     create_process((Function) test, test_argsa, 1, "test a", 4, 1);
    
   for(int i = 0; i < 10; i++){
       if(i % 2 == 0){
           char *  test_args[1] = {"a"};
           create_process((Function) test, test_args, 1, "test a", 4, 1);
       }
       else{
           char *  test_args[1] = {"b"};
           create_process((Function) test, test_args, 1, "test b", 4, 1);
       }
   }
    
    // sleep(1000000);
    idt_loader();
    while(1) {
        // hlt();
    } 

    return 0;
}

