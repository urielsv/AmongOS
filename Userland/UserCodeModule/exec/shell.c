// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <shell.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>
#include <commands.h>
#include <definitions.h>
#include <tests.h>

#define MAX_BUFFER_SIZE 1024


void test(){
    printf("Testeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeo\n");
    printf("jesteeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeo\n");
    printf("iesteeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeo\n");
    printf("esteeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeo\n");
}

static uint64_t test_mm_proc(uint64_t argc, char *argv[]) 
{
    test_mm(argc, argv);
}

static uint64_t test_process_proc(uint64_t argc, char *argv[]) 
{
    test_processes(argc, argv);
}

void shell() {

    print_header();

    //exec((void *) &test, NULL, 0, "test", 1, 1);

    //0x1000000 = 16777216 int
    char *argv_test_mm[] = {"16777216", NULL};
    exec((void *) &test_mm_proc, argv_test_mm, 1, "Testeo_de_memoria_:D", 1, 1);
    
    //exec((Function) test, NULL, 0, "test", 1, 1);


    char *argv_test_processes[] = {"10", NULL};
    //exec((void *) &test_processes_proc, NULL, 0, "test_processes", 1, 1);
    
    // exec((Function) print_ps1, NULL, 0, "testo", 10, 1);
    // exec((Function) print_header, NULL, 0, "test", 1, 1);

    // SHELL LOOP
    //char buff[MAX_BUFFER_SIZE];
   while (1) {
 //       print_ps1("user", "~");
 //        gets(buff, MAX_BUFFER_SIZE);
 //        execute_command(buff);
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



