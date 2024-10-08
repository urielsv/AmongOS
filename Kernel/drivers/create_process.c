#include "../include/create_process.h"
#include "../include/memman.h"
#include "../include/syscalls.h"

enum State { RUNNING, BLOCKED, READY, EXITED };
enum priority { LOW, LOW_MEDIUM, MEDIUM, HIGH_MEDIUM, HIGH };

typedef struct {

    uint64_t pid;
    uint64_t priority;
    enum State state;
    uint64_t stack_pointer;

} PCB_t;

typedef struct {

    uint64_t align; 
    uint64_t ss; 
    uint64_t rsp; 
    uint64_t rflags; 
    uint64_t cs; 
    uint64_t rip; 

} stack_frame_t;

typedef struct {

    stack_frame_t * stack_frame;
    PCB_t * pcb;

} process_t;

int create_process_stack(process_t *process) {

    process->stack_frame = (stack_frame_t *) mem_alloc(sizeof(stack_frame_t));

    if (process->stack_frame == NULL) {
        //TODO: aca va un mensaje de error
        return -1;
    }

    process->stack_frame->align = 0x0; //0x0 si stack frame es multiplo de 8 bytes
    process->stack_frame->ss = 0x0; 
    process->stack_frame->rsp = 0x0; // TODO: no es cero
    process->stack_frame->rflags = 0x202;
    process->stack_frame->cs = 0x8;
    process->stack_frame->rip = 0x0; // TODO: no es cero

    return 0;
}

int create_process_pcb(process_t *process) {

    process->pcb = (PCB_t *) mem_alloc(sizeof(PCB_t));

    if (process->pcb == NULL) {
        //TODO: aca va un mensaje de error
        return -1;
    }

    process->pcb->pid = pid();
    process->pcb->priority = LOW;
    process->pcb->state = READY;
    process->pcb->stack_pointer = (uint64_t) process->stack_frame->rsp; //TODO: revisar
    return 0;
}

int create_process(void) {

    process_t *process = (process_t *) mem_alloc(sizeof(process_t));

    if (process == NULL) {
        //TODO: aca va un mensaje de error
        return -1;
    }

    if (create_process_stack(process) == -1 || create_process_pcb(process) == -1) {
        //TODO: aca va un mensaje de error
        mem_free(process);
        return -1;
    }


    return 0;
}

