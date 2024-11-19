// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

