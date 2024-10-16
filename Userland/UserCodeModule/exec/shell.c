// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <shell.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>
#include <commands.h>
#include "../tests/include/test_mm.h"
#include "../tests/include/test_processes.h"
#include <definitions.h>

#define MAX_BUFFER_SIZE 1024

void test(){
    printf("Test\n");
}

void shell() {

    print_header();

    // char *argv_test_mm[] = {"0x1000000", NULL};
    // exec((Function) test_mm, argv_test_mm, 2, "test_mm", 1, 1);
    
    //exec((Function) test_processes, NULL, 0, "test_processes", 1, 1);
    
    exec((Function) test, NULL, 0, "test", 1, 1);

    // SHELL LOOP
    char buff[MAX_BUFFER_SIZE];
    while (1) {
        print_ps1("user", "~");
        gets(buff, MAX_BUFFER_SIZE);
        execute_command(buff);
        hlt();
    }
}



void print_header() {

    printf_color("Welcome to AmongOS Shell (SUShell) \n", COLOR_MAGENTA, COLOR_BLACK);
    putchar('\n');
    printf("To see the list of available commands, type 'help'\n");
    putchar('\n');
    putchar('\n');
}

void print_ps1(char *user, char *pwd) {

    printf_color("%s@AmongOS", COLOR_GREEN, COLOR_BLACK, user);
    printf_color("%s$ ", COLOR_MAGENTA, COLOR_BLACK, pwd);
}
