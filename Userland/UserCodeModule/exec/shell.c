// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <shell.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>
#include <definitions.h>
#include <tests.h>
#include <philosophers.h>

#define MAX_BUFFER_SIZE 1024

static void infinite_loop_proc(int argc, char *argv[]) {
    printf("Argc: %d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }

    while (1) {
        //para cumplir con la consigna
        sleep(3000);
        printf("Infinite loop from pid: %d\n", get_pid());
    }
}

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
    {"mem", "Muestra la memoria total, usada y libre", (Function) print_mem_info},
    {"infinite_loop", "Starts an infinite loop", (Function) infinite_loop_proc},
    {"philos", "start the classic philosophers dilemma", (Function) philos_proc},
    {"cat", "Imprime el stdin tal como lo recibe", (Function) cat},
    {"wc", "Cuenta la cantidad de lineas del stdin", (Function) wc},
    {"block","Bloquea un proceso", (Function) block_proc},
    {"unblock", "Desbloquea un proceso", (Function) unblock_proc},
    {"filter", "Filtra las vocales del stdin", (Function) filter}
};


static void parse_buffer(char *buff, parsed_input_t *parsed);
static int execute_command(parsed_input_t *parsed);
static int validate_pid(char *pid_str);
static void print_header();
static void print_ps1(char *user, char *pwd);

void shell() {

    print_header();
    print_ps1("user", "~");

    // SHELL LOOP
    char buff[MAX_BUFFER_SIZE];
    parsed_input_t parsed;
    memset(&parsed, 0, sizeof(parsed_input_t));
    while (1) {
        gets(buff, MAX_BUFFER_SIZE);
        parse_buffer(buff, &parsed);
        execute_command(&parsed);
        print_ps1("user", "~");
   }
}



static void parse_buffer(char *buff, parsed_input_t *parsed) {
    if (!buff || !parsed) return;
    
    memset(parsed, 0, sizeof(parsed_input_t));
    
    int len = strlen(buff);
    while (len > 0 && (buff[len-1] == ' ' || buff[len-1] == '\n')) {
        buff[--len] = '\0';
    }
    
    if (len > 0 && buff[len-1] == '&') {
        parsed->is_bg = 1;
        buff[--len] = '\0';
        while (len > 0 && buff[len-1] == ' ') {
            buff[--len] = '\0';
        }
    }
    
    char *cmd_str = buff;
    char *pipe_pos;
    
    while (cmd_str && *cmd_str && parsed->cmd_count < MAX_CMDS) {
        while (*cmd_str == ' ') cmd_str++;
        if (!*cmd_str) break;
        
        pipe_pos = strchr(cmd_str, '|');
        if (pipe_pos) {
            *pipe_pos = '\0';
        }
        
        command_input_t *current_cmd = &parsed->cmds[parsed->cmd_count];
        
        char *token = cmd_str;
        char *next_token = NULL;
        int arg_count = 0;

        while (*token == ' ') token++;
        next_token = strchr(token, ' ');
        if (next_token) {
            *next_token = '\0';
            next_token++;
        }
        current_cmd->cmd = token;
        
        if (next_token) {
            token = next_token;
            while (*token && arg_count < MAX_ARGS - 1) { 
                while (*token == ' ') token++;
                if (!*token) break;
                
                next_token = strchr(token, ' ');
                if (next_token) {
                    *next_token = '\0';
                    next_token++;
                }
                
                current_cmd->argv[arg_count++] = token;
                
                if (!next_token) break;
                token = next_token;
            }
        }
        
        current_cmd->argc = arg_count;
        current_cmd->argv[arg_count] = NULL;
        
        parsed->cmd_count++;
        
        if (pipe_pos) {
            cmd_str = pipe_pos + 1;
        } else {
            break;
        }
    }
}


static int execute_command(parsed_input_t *parsed) {
    if (!parsed || parsed->cmd_count == 0) return 0;

    if (parsed->cmd_count == 1) {
        command_input_t *current = &parsed->cmds[0];
        
        for (int i = 0; i < sizeof(commands) / sizeof(command_t); i++) {
            if (strcmp(commands[i].name, current->cmd) == 0) {
                int pid;
                
                if (parsed->is_bg) {
                    pid = exec((void *)commands[i].cmd, current->argv, current->argc, 
                             commands[i].name, DEFAULT_PRIORITY);
                
                    block(pid);
                  
                    change_process_fd(pid, STDIN, DEV_NULL);
                    change_process_fd(pid, STDOUT, DEV_NULL);
                    change_process_fd(pid, STDERR, DEV_NULL);
                
                    
                    unblock(pid);
                    //yield();
                    printf("[%d] Running in background\n", pid);
                } else {
                    pid = exec((void *)commands[i].cmd, current->argv, current->argc, 
                             commands[i].name, DEFAULT_PRIORITY);
                    waitpid(pid);
                }
                return 0;
            }
        }
        printf("SUShell: '%s' command not found", current->cmd);
        return 0;
    }

    uint16_t pipe_id = create_pipe();

    int pids[MAX_CMDS] = {0};
    
    for (int i = 0; i < parsed->cmd_count; i++) {
        command_input_t *current = &parsed->cmds[i];
        int found = 0;
        for (int j = 0; j < sizeof(commands) / sizeof(command_t); j++) {
            if (strcmp(commands[j].name, current->cmd) == 0) {
                pids[i] = exec((void *)commands[j].cmd, current->argv, current->argc, 
                             commands[j].name, DEFAULT_PRIORITY);
                block(pids[i]);
                
                if (i == 0) {
                    open_pipe(pids[i], pipe_id, WRITE_MODE);
                    change_process_fd(pids[i], STDOUT, pipe_id);
                }

                if (i == 1) {
                    open_pipe(pids[i], pipe_id, READ_MODE);
                    change_process_fd(pids[i], STDIN, pipe_id);
                }

                found = 1;
                break;
            }
        }
        if (!found) {
            printf("SUShell: '%s' command not found", current->cmd);
            return 0;
        }
    }

    for (int i = 0; i < parsed->cmd_count; i++) {
        unblock(pids[i]);
    }
    
    if (!parsed->is_bg) {
        waitpid(pids[parsed->cmd_count - 1]);
    } else {
        uint16_t null_pipe = create_pipe();
        if (null_pipe != -1) {
            open_pipe(pids[0], null_pipe, READ_MODE);
            printf("[%d] Pipe chain running in background\n", pids[0]);
        }
    }

    close_pipe(pipe_id);
    
    return 0;
}


void ps() {
    printf("PID\t\tPRIO\t\t\t\tSTATE\t\t\t\t\tNAME\t\t\tSP\t\t\t\t\tBP\n");
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
                    state = "WAITING FOR CHILD";
                    break;
            }
            char *priority = "";
            switch (process->priority) {
                case LOW:
                    priority = "LOW        ";
                    break;
                case LOW_MEDIUM:
                    priority = "LOW MEDIUM ";
                    break;
                case MEDIUM:
                    priority = "MEDIUM     ";
                    break;
                case HIGH_MEDIUM:
                    priority = "HIGH MEDIUM";
                    break;
                case HIGH:
                    priority = "HIGH       ";
                    break;
            }
            printf("%d\t\t%s\t\t%s\t\t%s %s\t\t%s\t%s", process->pid, priority, state, process->name, process->argv, process->stack_pointer, process->base_pointer);
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

    printf("command_t: Description\n");
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

int philos_proc(int argc, char *argv[])
{
    return run_philosophers(argc, argv);
}

void block_proc(int argc, char *argv[])
{
    if (argc != 1)
    {
        printf("Usage: block <pid>");
        return;
    }

    int32_t pid = -1;
    if ((pid = validate_pid(argv[0])) < 0)
    {
        printf("Error: invalid pid %d", argv[0]);
        return;
    }

    block(pid);
}


void unblock_proc(int argc, char *argv[])
{
    if (argc != 1)
    {
        printf("Usage: unblock <pid>");
        return;
    }

    int32_t pid = -1;
    if ((pid = validate_pid(argv[0])) < 0)
    {
        printf("Error: invalid pid %d", argv[0]);
        return;
    }

    unblock(pid);
}



int cat(int argc, char *argv[]) {

    for (int i = 0; i < argc; i++) {
        printf("%s ", argv[i]);
    }
	return 0;
}

int wc(int argc, char **argv) {

    int buffer_count = 0;
    uint64_t width, height;
    screen_info(&width, &height);

    for (int i = 0; i < argc; i++) {
        int index = 0;
        while(argv[i][index++] != '\0') {
            buffer_count++;
        }
        buffer_count++;
    }
    // no es responsiva, pero bueno, esto ya tiene q ver con el tp anterior de arqui
    printf("La cantidad de lineas es: %d\n", buffer_count/128+1);
	return 0;
}


int filter(int argc, char **argv) {

    if (argc < 1) {
        printf("Usage: filter <text>\n");
        return 0;
    }
    for (int i = 0; i < argc; i++) {
        for (int count_buffer = 0; argv[i][count_buffer] != '\0'; count_buffer++) {
            if (toLower(argv[i][count_buffer]) != 'a' && 
                toLower(argv[i][count_buffer]) != 'e' && 
                toLower(argv[i][count_buffer]) != 'i' && 
                toLower(argv[i][count_buffer]) != 'o' && 
                toLower(argv[i][count_buffer]) != 'u') {
                putchar(argv[i][count_buffer]);
            }
        }
        putchar(' ');
    }
    return 0;
}

size_t print_mem_info() {
    size_t *info = memory_info();
    printf("\nTotal: %d, Used: %d, Free: %d\n", info[0], info[1], info[2]);
    return 0;
}
