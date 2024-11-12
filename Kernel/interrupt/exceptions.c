// this is a personal academic project. dear pvs-studio, please check it.
// pvs-studio static code analyzer for c, c++ and c#: http://www.viva64.com
#include <exceptions.h>
// #include <io.h>
#include <stdio.h>
#include <naive_console.h>

#define error_fgcolor 0xff0000
#define error_bgcolor 0x000000
#define zero_division 0
#define invalid_opcode  6
#define buff_size 30

static int flag = 0;
static void restore_state(uint64_t* stack);

typedef struct{
    uint64_t ip, sp, bp;
} restore_point;

static restore_point rp;

// length of the registers array is 17
static char * regs[] = {
        "r15 = 0x", "r14 = 0x", "r13 = 0x", "r12 = 0x", "r11 = 0x", "r10 = 0x", "r9 = 0x", "r8 = 0x", "rsi = 0x",
         "rdi = 0x", "rbp = 0x", "rdx = 0x", "rcx = 0x", "rbx = 0x", "rax = 0x", "ip = 0x", "rsp = 0x",  
};


static uint32_t registers_len = sizeof(regs)/sizeof(regs[0]);


void exception_dispatcher(uint32_t exception, uint64_t* stack){
    switch(exception){
        case zero_division:
        break;
        case invalid_opcode:
        break;
    }
    flag = 1;
    print_regs(stack);
    restore_state(stack);

}


void print_regs(uint64_t * stack){
    
    if (stack == 0){
        return;
    }
    
 
    for(int i = 0; i < registers_len - 2; i++){ 
        char buff[16];
        uint_to_base(stack[i],buff,16);
    }
    if(flag == 0){
        char buff1[16];
        uint_to_base(stack[16], buff1,16);

    }else if(flag == 1){
        char buff1[16];
        uint_to_base(rp.ip, buff1,16);

        flag = 0;
    }
    
    char buff2[16]; 
    uint_to_base(stack[registers_len + 1], buff2, 16);  

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
    char buffer[30];
	uint_to_base(stack[registers_len - 2], buffer, 16);
	uint_to_base(rp.sp, buffer, 16);
    
	// restauramos los valores
	stack[registers_len - 2] = rp.ip;  // rip
	stack[registers_len + 1] = rp.sp;  // rsp
	stack[registers_len - 7] = rp.bp;  // rsp
}
