#ifndef _CREATE_PROCESS_H
#define _CREATE_PROCESS_H

#include <stdint.h>
#include <stdio.h>

int create_process(const char * path, int args, char *argv[]);
int kill_process(uint64_t pid);
int block_process(uint64_t pid);
int unblock_process(uint64_t pid);

#endif