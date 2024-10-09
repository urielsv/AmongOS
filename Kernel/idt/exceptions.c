// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <exceptions.h>
#include <io.h>
#include <naiveConsole.h>

#define ERROR_FGCOLOR 0xFF0000
#define ERROR_BGCOLOR 0x000000
#define ZERO_DIVISION 0
#define INVALID_OPCODE  6
#define BUFF_SIZE 30

static int flag = 0;
static void restore_state(uint64_t* stack);

typedef struct{
    uint64_t ip, sp, bp;
} restore_point;

static restore_point rp;

// Length of the registers array is 17
static char * regs[] = {
        "R15 = 0x", "R14 = 0x", "R13 = 0x", "R12 = 0x", "R11 = 0x", "R10 = 0x", "R9 = 0x", "R8 = 0x", "RSI = 0x",
         "RDI = 0x", "RBP = 0x", "RDX = 0x", "RCX = 0x", "RBX = 0x", "RAX = 0x", "IP = 0x", "RSP = 0x",  
};

//static char buffer[BUFF_SIZE];

static uint32_t registers_len = sizeof(regs)/sizeof(regs[0]);


void exception_dispatcher(uint32_t exception, uint64_t* stack){
    switch(exception){
        case ZERO_DIVISION:
                printf_color("\nZero Division Error Exception\n",ERROR_FGCOLOR, ERROR_BGCOLOR);
        break;
        case INVALID_OPCODE:
                printf_color("\nInvadid Opcode Exception\n", ERROR_FGCOLOR,ERROR_BGCOLOR);
        break;
    }
    flag = 1;
    print_regs(stack);
    restore_state(stack);

}


void print_regs(uint64_t * stack){
    
    if (stack == 0){
        printf("\nError. Before print registers, press the key '0'\n");
        return;
    }
    
    putchar('\n'); 
 
    for(int i = 0; i < registers_len - 2; i++){ 
        printf(regs[i]);
        char buff[16];
        uintToBase(stack[i],buff,16);
        printf(buff);
        putchar('\n');
    }
    if(flag == 0){
        printf(regs[registers_len -2]);
        char buff1[16];
        uintToBase(stack[16], buff1,16);
        printf(buff1);
        putchar('\n');

        // test rp value
        // char buff2[16];
        // uintToBase(rp.sp, buff2, 16);
        // printf(buff2);
        // putchar('\n');
    }else if(flag == 1){
        printf(regs[registers_len -2]);
        char buff1[16];
        uintToBase(rp.ip, buff1,16);
        printf(buff1);
        putchar('\n');

        flag = 0;
    }
    printf(regs[registers_len - 1]);
    
    char buff2[16]; 
    uintToBase(stack[registers_len + 1], buff2, 16);  
    printf(buff2); 
    putchar('\n');

}

void
set_restore_point(uint64_t ip, uint64_t sp, uint64_t bp)
{
	rp.ip = ip;
	rp.sp = sp;
	rp.bp = bp;
}


static void
restore_state(uint64_t* stack)
{
    
	printf("Restoring state from: IP=0x");
    char buffer[30];
	uintToBase(stack[registers_len - 2], buffer, 16);
	printf(buffer);
	printf("  SP=0x");
	uintToBase(rp.sp, buffer, 16);
	printf(buffer);
	printf("\n\n");
    
	// restauramos los valores
	stack[registers_len - 2] = rp.ip;  // RIP
	stack[registers_len + 1] = rp.sp;  // RSP
	stack[registers_len - 7] = rp.bp;  // RSP
}
