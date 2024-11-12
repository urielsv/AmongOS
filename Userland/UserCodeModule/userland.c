// this is a personal academic project. dear pvs-studio, please check it.
// pvs-studio static code analyzer for c, c++ and c#: http://www.viva64.com
/* sample_code_module.c */
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
