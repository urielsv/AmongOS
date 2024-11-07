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
    {"test_synchro", "testeo de sincronizacion", (Function) test_synchro_proc},
    {"kill", "Kill a process <pid>.", (Function) kill_proc},
    {"nice", "Change the scheduling priority of a process <pid> to <priority>.", (Function) nice},
    {"ps", "List all processes", (Function) ps},


};

static void parse_buffer(char *buff, char **cmd, char **argv, int *argc);
static int validate_pid(char *pid_str);
static void print_header();
static void print_ps1(char *user, char *pwd);



void shell()
{

    print_header();
    print_ps1("user", "~");

    // SHELL LOOP
    char buff[MAX_BUFFER_SIZE];
    char *cmd = NULL;
    char **argv = {0};
    int argc = 0;
   while (1) {
        gets(buff, MAX_BUFFER_SIZE);
        parse_buffer(buff, &cmd, argv, &argc);
        execute_command(cmd, argv, argc);
        print_ps1("user", "~");
   }
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
            int pid = exec((void *)commands[i].cmd, argv, argc, commands[i].name, DEFAULT_PRIORITY);
            waitpid(pid);
            return 0;
        }
    }
    printf("SUShell: '%s' command not found", cmd);
    return 0;
}


void ps(char *argv, int argc) {
    if (argc != 0) {
        printf("Usage: ps");
        return;
    }
    printf("PID\t\tPRIO\t\t\t\tSTATE\t\t\t\t\tNAME\n");
    // TODO: MAke this more efficient sicne we are iterating over all the processes
    for (int i = 0; i < MAX_PROCESSES; i++) {
        process_snapshot_t *process = process_snapshot(i);
        if (process != NULL) {
            char *state = "";
            switch (process->state) {
                case BLOCKED:
                    state = "BLOCKED          ";
                    break;
                case READY:
                    state = "READY            ";
                    break;
                case KILLED:
                    state = "KILLED           ";
                    break;
                case RUNNING:
                    state = "RUNNING          ";
                    break;
                case WAITING_FOR_CHILD:
                    state = "WAITING_FOR_CHILD";
                    break;
            }
            char *priority = "";
            switch (process->priority) {
                case LOW:
                    priority = "LOW        ";
                    break;
                case LOW_MEDIUM:
                    priority = "LOW_MEDIUM ";
                    break;
                case MEDIUM:
                    priority = "MEDIUM     ";
                    break;
                case HIGH_MEDIUM:
                    priority = "HIGH_MEDIUM";
                    break;
                case HIGH:
                    priority = "HIGH       ";
                    break;
            }
            printf("%d\t\t%s\t\t%s\t\t%s %s", process->pid, priority, state, process->name, process->argv);
            if (i < MAX_PROCESSES - 1) {
                printf("\n");
            }
        }

    }
}

static int validate_pid(char *pid_str) {
    uint32_t pid = atoi(pid_str);
    return process_exists(pid) ? pid : -1;
}

void nice(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: nice <pid> <priority>\n");
        return;
    }
   
    int32_t pid = -1;
    if ((pid = validate_pid(argv[0])) < 0) {
        printf("Error: invalid pid %d\n", argv[0]);
        return;
    }

    int prio = atoi(argv[1]);
    if (prio < LOW || prio > HIGH) {
        printf("Error: invalid priority %d\n", prio);
        return;
    }

    set_priority(pid, prio);
}

void kill_proc(int argc, char *argv[]) {
    if (argc != 1) {
        printf("Usage: kill <pid>");
        return;
    }
   
    int32_t pid = -1;
    if ((pid = validate_pid(argv[0])) < 0) {
        printf("Error: invalid pid %d", argv[0]);
        return;
    }

    kill(pid);
}


inline void print_header()
{

    printf_color("Welcome to AmongOS Shell (SUShell) \n", COLOR_MAGENTA, COLOR_BLACK);
    putchar('\n');
    printf("To see the list of available commands, type 'help'\n");
    putchar('\n');
}

inline void print_ps1(char *user, char *pwd)
{

    printf_color("\n%s@AmongOS", COLOR_GREEN, COLOR_BLACK, user);
    printf_color("%s$ ", COLOR_MAGENTA, COLOR_BLACK, pwd);
}

int print_help() {

    printf("Command: Description\n");
    int size = sizeof(commands) / sizeof(command_t);
    for (int i = 1; i < size; i++) {
        printf("%s: %s", commands[i].name, commands[i].description);
        if (i < size - 1) {
            putchar('\n');
        }
    }
    return 0;
}



int screen() {
    uint64_t width, height;
    screen_info(&width, &height);
    printf("W:%d, H:%d\n", width, height);
    return 0;
}

int font() {
    int num;
    scanf("New size: %d", &num);
    if (num <= 0 || num >= 6) {
        printf("\nInvalid size\n");
    } else {
        font_size(num);
        clear(COLOR_BLACK);
    }
    return 0;
}

int print_amongus() {
    printf_color("........ooooooooo.......\n", COLOR_RED, COLOR_BLACK);
    printf_color("......oo.....oooooo.....\n", COLOR_RED, COLOR_BLACK);
    printf_color("......oo...oo......oo...\n", COLOR_RED, COLOR_BLACK);
    printf_color("......oo.....oooooo.....\n", COLOR_RED, COLOR_BLACK);
    printf_color("......oo.......oo.......\n", COLOR_RED, COLOR_BLACK);
    printf_color("......oo...oo..oo.......\n", COLOR_RED, COLOR_BLACK);
    printf_color("......oo...oo..oo.......\n", COLOR_RED, COLOR_BLACK);
    printf_color("......ooo..oo.ooo.......", COLOR_RED, COLOR_BLACK);
    return 0;
}

int print_random() {
    printf("%d", random(1203929));
    return 0;
}

int print_time() {
    printf("%s", time());
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
