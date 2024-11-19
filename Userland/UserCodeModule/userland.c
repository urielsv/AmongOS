// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <shell.h>

char *v = (char *)0xb8000 + 79 * 2;

static int var1 = 0;
static int var2 = 0;

int main() {
    // start the shell

    shell();

    *v = 'x';
    *(v + 1) = 0x74;

    if (var1 == 0 && var2 == 0)
        return 0xdeadc0de;

    return 0xdeadbeef;
}
