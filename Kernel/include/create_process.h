#ifndef _CREATE_PROCESS_H
#define _CREATE_PROCESS_H

#include <stdint.h>
#include <stdio.h>

//puntero a la funcion que se va a ejecutar
typedef int (*Function)(int argc, char **args);

int create_process(Function code, int argc, char **argv);
int kill_process(uint64_t pid);
int block_process(uint64_t pid);
int unblock_process(uint64_t pid);
extern void * create_process_stack_frame(void * rip, void * rsp, void * argv);

#endif