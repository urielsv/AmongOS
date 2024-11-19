// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// this is a personal academic project. dear pvs-studio, please check it.
// pvs-studio static code analyzer for c, c++ and c#: http://www.viva64.com
/* _loader.c */
#include <stdint.h>
#include <stdlib.h>

extern char bss;
extern char end_of_binary;

int main();

void * memset(void * destiny, int32_t c, uint64_t length);

int _start() {
	//clean bss
	memset(&bss, 0, &end_of_binary - &bss);

	return main();

}

