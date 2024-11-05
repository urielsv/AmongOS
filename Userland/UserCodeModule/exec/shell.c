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

command_t commands[] = {
    {"help", "Shows this help", (Function) print_help},
    {"clear", "Clears the screen", (Function) print_clear},
    {"time", "Shows the current time", (Function) print_time},
    {"random", "Shows a random number", (Function) print_random},
    {"screen", "Shows the screen size", (Function) screen},
    {"font", "Change font size (1-5)", (Function) font},
    {"exit", "Exits the shell", NULL},
    {"amongus", "Prints Among Us", (Function) print_amongus},
    {"snake", "Starts the snake game", (Function) snake},
    {"regs", "printea los registros", (Function) print_regs},
    {"zero", "testea la excepcion de division por cero", (Function) test_zero},
    {"opcode", "testea la excepcion de opcode invalido", (Function) opcode},
    {"test_mm", "testeo de memoria", (Function) test_mm_proc},
    {"test_processes", "testeo de procesos", (Function) test_processes_proc},
    {"test_prio", "testeo de prioridad", (Function) test_prio_proc},
    {"test_synchro", "testeo de sincronizacion", (Function) test_synchro_proc}
};

static void parse_buffer(char *buff, char **cmd, char **argv, int *argc);


void shell()
{

    print_header();


    // MM TES
    // 0x1000000 (16) = 16777216 (10)
    // char *argv_test_mm[] = {"16777216", NULL};
    // exec((void *) &test_mm_proc, argv_test_mm, 1, "Testeo_de_memoria_:D", 1, 1);

    // PROCESSES TEST
     //char *argv_test_processes[] = {"10", NULL};
     //exec((void *)&test_processes_proc, argv_test_processes, 1, "test_processes", 1);

    //PRIO TEST
    //exec((void *)&test_prio_proc, NULL, 0, "test_prio", 1);

    // SYNCHRO TEST

    // char *argv_test_sync[] = {"2", "-1", NULL};
    // exec((void *)&test_synchro_proc, argv_test_sync, 2, "test_synchro", 1);

    // printf("----------------------------------------------------\n"); 
    //char *argv_test_sync1[] = {"4", "0", NULL};
    //exec((void *)&test_synchro_proc, argv_test_sync1, 2, "test_synchro", 1);
    

    // clear(0x0);
    // print_ps1("user", "~");
    exec((void *)&print_amongus, 0, 0, "amonus", DEFAULT_PRIORITY);
    exec((void *)&print_help, 0, 0, "help", DEFAULT_PRIORITY);
    exec((void *)&print_help, 0, 0, "help", DEFAULT_PRIORITY);
    //execute_command(commands[0].name, NULL, 0);
    // SHELL LOOP
    char buff[MAX_BUFFER_SIZE];
    char *cmd = NULL;
    char **argv = {0};
    int argc = 0;
   while (1) {
        print_ps1("user", "~");
        gets(buff, MAX_BUFFER_SIZE);
        parse_buffer(buff, &cmd, argv, &argc);
        execute_command(cmd, argv, argc);
   }
   printf("Bye shell\n");
}

static void parse_buffer(char *buff, char **cmd, char **argv, int *argc) {
    int i = 0;
    *argc = 0;
    while (buff[i] != ' ' && buff[i] != '\0') {
        i++;
    }
    if (buff[i] == '\0') {
        *cmd = buff;
        *argv = NULL;
    } else {
        buff[i] = '\0';
        *cmd = buff;
        *argv = buff + i + 1;
        
        // Count arguments
        char *arg_ptr = *argv;
        while (*arg_ptr != '\0') {
            while (*arg_ptr == ' ') {
                arg_ptr++;
            }
            if (*arg_ptr != '\0') {
                (*argc)++;
                while (*arg_ptr != ' ' && *arg_ptr != '\0') {
                    arg_ptr++;
                }
            }
        }
    }
}


int execute_command(char *cmd, char **argv, int argc) {
    for (int i = 0; i < sizeof(commands) / sizeof(command_t); i++) {
        if (strcmp(commands[i].name, cmd) == 0) {
            exec((void *)commands[i].cmd, 0, 0, commands[i].name, DEFAULT_PRIORITY);
            return 0;
        }
    }
    printf("\nSUShell: '%s' command not found\n", cmd);
    return 0;
}


void print_header()
{

    printf_color("Welcome to AmongOS Shell (SUShell) \n", COLOR_MAGENTA, COLOR_BLACK);
    putchar('\n');
    printf("To see the list of available commands, type 'help'\n");
    putchar('\n');
    putchar('\n');
}

void print_ps1(char *user, char *pwd)
{

    printf_color("%s@AmongOS", COLOR_GREEN, COLOR_BLACK, user);
    printf_color("%s$ ", COLOR_MAGENTA, COLOR_BLACK, pwd);
}

int print_help() {

    printf("\nCommand: Description\n");
    for (int i = 1; i < sizeof(commands) / sizeof(command_t); i++) {
        printf("%s: %s\n", commands[i].name, commands[i].description);
    }
    return 0;
}



int screen() {
    uint64_t width, height;
    screen_info(&width, &height);
    printf("\nW:%d, H:%d\n", width, height);
    return 0;
}

int font() {
    int num;
    scanf("\nNew size: %d", &num);
    if (num <= 0 || num >= 6) {
        printf("\nInvalid size\n");
    } else {
        font_size(num);
        clear(COLOR_BLACK);
    }
    return 0;
}

int print_amongus() {
    printf_color("\n........ooooooooo.......\n", COLOR_RED, COLOR_BLACK);
    printf_color("......oo.....oooooo.....\n", COLOR_RED, COLOR_BLACK);
    printf_color("......oo...oo......oo...\n", COLOR_RED, COLOR_BLACK);
    printf_color("......oo.....oooooo.....\n", COLOR_RED, COLOR_BLACK);
    printf_color("......oo.......oo.......\n", COLOR_RED, COLOR_BLACK);
    printf_color("......oo...oo..oo.......\n", COLOR_RED, COLOR_BLACK);
    printf_color("......oo...oo..oo.......\n", COLOR_RED, COLOR_BLACK);
    printf_color("......ooo..oo.ooo.......\n", COLOR_RED, COLOR_BLACK);
    return 0;
}

int print_random() {
    printf("\n%d\n", random(1203929));
    return 0;
}

int print_time() {
    printf("\n%s\n", time());
    return 0;
}

int print_clear() {
    clear(COLOR_BLACK);
    return 0;
}

int print_regs() {
    sys_registers();
    return 0;
}

int test_zero(){
    test_exc_zero();
    return 0;
}

int opcode() {
    test_exc_invalid_opcode();
    return 0;
}

uint64_t test_mm_proc(uint64_t argc, char *argv[])
{
    return test_mm(argc, argv);
}

uint64_t test_processes_proc(uint64_t argc, char *argv[])
{
    return test_processes(argc, argv);
}

int test_prio_proc()
{
    test_prio();
    return 0;
}

int test_synchro_proc(uint64_t argc, char *argv[])
{
    test_sync(argc, argv); //{n, use_sem, 0}
    return 0;
}
