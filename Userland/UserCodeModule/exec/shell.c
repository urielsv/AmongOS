// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// this is a personal academic project. dear pvs-studio, please check it.
// pvs-studio static code analyzer for c, c++ and c#: http://www.viva64.com
#include <shell.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>
#include <definitions.h>
#include <tests.h>
#include <philosophers.h>

#define MAX_BUFFER_SIZE 1024

static void infinite_loop_proc(int argc, char *argv[]) {

    while (1) {
        sleep(5000);
        printf("infinite loop from pid: %d\n", get_pid());
    }
}

command_t commands[] = {
    {"help", "shows this help", (function) print_help},
    {"clear", "clears the screen", (function) print_clear},
    {"time", "shows the current time", (function) print_time},
    {"random", "shows a random number", (function) print_random},
    {"screen", "shows the screen size", (function) screen},
    {"font", "change font size (1-5)", (function) font},
    {"exit", "exits the shell", NULL},
    {"amongus", "prints among us", (function) print_amongus},
    {"snake", "starts the snake game", (function) snake},
    {"regs", "print registers", (function) print_regs},
    {"zero", "test zero exception", (function) test_zero},
    {"opcode", "test invalid opcode exception", (function) opcode},
    {"test_mm", "test memory manager", (function) test_mm_proc},
    {"test_processes", "test processes", (function) test_processes_proc},
    {"test_prio", "test priority", (function) test_prio_proc},
    {"test_synchro", "test synchronization", (function) test_synchro_proc},
    {"kill", "kill a process <pid>.", (function) kill_proc},
    {"block","block a process", (function) block_proc},
    {"unblock", "unblock a process", (function) unblock_proc},
    {"nice", "change the scheduling priority of a process <pid> to <priority>.", (function) nice},
    {"ps", "list all processes", (function) ps},
    {"mem", "show total, used and free memory", (function) print_mem_info},
    {"infinite_loop", "starts an infinite loop", (function) infinite_loop_proc},
    {"philos", "start the classic philosophers dilemma", (function) philos_proc},
    {"cat", "prints the STDIN as received", (function) cat},
    {"wc", "counts the number of lines of the STDIN", (function) wc},
    {"filter", "filter the vowels of the STDIN", (function) filter},
};


static void parse_buffer(char *buff, parsed_input_t *parsed);
static int execute_command(parsed_input_t *parsed);
static int validate_pid(char *pid_str);
static void print_header();
static void print_ps1(char *user, char *pwd);

void shell() {

    print_header();
    print_ps1("user", "~");

    // shell loop
    char buff[MAX_BUFFER_SIZE]={0};
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
    
    while (cmd_str && *cmd_str && parsed->cmd_count < max_cmds) {
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
            while (*token && arg_count < max_args - 1) { 
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
                    set_bg(pid);
                    unblock(pid);
                    printf("[%d] running in background\n", pid);
                } else {
                    pid = exec((void *)commands[i].cmd, current->argv, current->argc, 
                             commands[i].name, DEFAULT_PRIORITY);
                    waitpid(pid);
                }
                return 0;
            }
        }
        printf("sus_shell: '%s' command not found", current->cmd);
        return 0;
    }

    uint16_t pipe_id = create_pipe();

    int pids[max_cmds] = {0};
    
    for (int i = 0; i < parsed->cmd_count; i++) {
        command_input_t *current = &parsed->cmds[i];
        int found = 0;
        for (int j = 0; j < sizeof(commands) / sizeof(command_t); j++) {
            if (strcmp(commands[j].name, current->cmd) == 0) {
                pids[i] = exec((void *)commands[j].cmd, current->argv, current->argc, 
                             commands[j].name, DEFAULT_PRIORITY);
                block(pids[i]);
                
                if (i == 0) {
                    open_pipe(pids[i], pipe_id, write_mode);
                    change_process_fd(pids[i], STDOUT, pipe_id);
                }

                if (i == 1) {
                    open_pipe(pids[i], pipe_id, read_mode);
                    change_process_fd(pids[i], STDIN, pipe_id);
                }

                found = 1;
                break;
            }
        }
        if (!found) {
            printf("sus_shell: '%s' command not found", current->cmd);
            return 0;
        }
    }

    for (int i = 0; i < parsed->cmd_count; i++) {
        unblock(pids[i]);
    }
    
    if (!parsed->is_bg) {
        waitpid(pids[parsed->cmd_count - 1]);
        // close_pipe(pipe_id);
    } else {
        uint16_t NULL_pipe = create_pipe();
        if (NULL_pipe != -1) {
            open_pipe(pids[0], NULL_pipe, read_mode);
            printf("[%d] pipe chain running in background\n", pids[0]);
        }
    }

    close_pipe(pipe_id);
    
    return 0;
}


void ps() {
    printf("PID\t\tPRIO\t\t\t\tSTATE\t\t\t\t\tNAME\t\t\t\tSP\t\t\t\tBP\t\tFG\n");
    for (int i = 0; i <MAX_PROCESSES; i++) {
        process_snapshot_t *process = process_snapshot(i);
        if (process != NULL) {
            char *state = "";
            switch (process->state) {
                case blocked:
                    state = "blocked          ";
                    break;
                case ready:
                    state = "ready            ";
                    break;
                case killed:
                    state = "killed           ";
                    break;
                case running:
                    state = "running          ";
                    break;
                case waiting_for_child:
                    state = "waiting for child";
                    break;
            }
            char *priority = "";
            switch (process->priority) {
                case low:
                    priority = "low        ";
                    break;
                case low_medium:
                    priority = "low medium ";
                    break;
                case medium:
                    priority = "medium     ";
                    break;
                case high_medium:
                    priority = "high medium";
                    break;
                case high:
                    priority = "high       ";
                    break;
            }
            char *fg = "";
            if (process->fg == background) {
                fg = "bg";
            } else {
                fg = "fg";
            }
            printf("%d\t\t%s\t\t%s\t\t%s %s\t\t\t%s\t%s\t\t%s", process->pid, priority, state, process->name, process->argv, 
            process->stack_pointer, process->base_pointer, fg);
            if (i <MAX_PROCESSES - 1) {
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
        printf("usage: nice <pid> <priority>\n");
        return;
    }
   
    int32_t pid = -1;
    if ((pid = validate_pid(argv[0])) < 0) {
        printf("error: invalid pid %d\n", argv[0]);
        return;
    }

    int prio = atoi(argv[1]);
    if (prio < low || prio > high) {
        printf("error: invalid priority %d\n", prio);
        return;
    }

    set_priority(pid, prio);
}

void kill_proc(int argc, char *argv[]) {
    if (argc != 1) {
        printf("usage: kill <pid>");
        return;
    }
   
    int32_t pid = -1;
    if ((pid = validate_pid(argv[0])) < 0) {
        printf("error: invalid pid %d", argv[0]);
        return;
    }

    kill(pid);
}


inline void print_header()
{

    printf_color("welcome to among_os shell (su_shell) \n", color_magenta, color_black);
    putchar('\n');
    printf("to see the list of available commands, type 'help'\n");
    putchar('\n');
}

inline void print_ps1(char *user, char *pwd)
{

    printf_color("\n%s@among_os", color_green, color_black, user);
    printf_color("%s$ ", color_magenta, color_black, pwd);
}

int print_help() {

    printf("command_t: description\n");
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
    printf("w:%d, h:%d\n", width, height);
    return 0;
}

int font() {
    int num;
    scanf("new size: %d", &num);
    if (num <= 0 || num >= 6) {
        printf("\n_invalid size\n");
    } else {
        font_size(num);
        clear(color_black);
    }
    return 0;
}

int print_amongus() {
    printf_color("........ooooooooo.......\n", color_red, color_black);
    printf_color("......oo.....oooooo.....\n", color_red, color_black);
    printf_color("......oo...oo......oo...\n", color_red, color_black);
    printf_color("......oo.....oooooo.....\n", color_red, color_black);
    printf_color("......oo.......oo.......\n", color_red, color_black);
    printf_color("......oo...oo..oo.......\n", color_red, color_black);
    printf_color("......oo...oo..oo.......\n", color_red, color_black);
    printf_color("......ooo..oo.ooo.......", color_red, color_black);
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
    clear(color_black);
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
    if (argc != 2)
    {
        printf("usage: test_synchro <n> <sem>");
        return 0;
    }
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
        printf("usage: block <pid>");
        return;
    }

    int32_t pid = -1;
    if ((pid = validate_pid(argv[0])) < 0)
    {
        printf("error: invalid pid %d", argv[0]);
        return;
    }

    block(pid);
}


void unblock_proc(int argc, char *argv[])
{
    if (argc != 1)
    {
        printf("usage: unblock <pid>");
        return;
    }

    int32_t pid = -1;
    if ((pid = validate_pid(argv[0])) < 0)
    {
        printf("error: invalid pid %d", argv[0]);
        return;
    }

    unblock(pid);
}



int cat(int argc, char *argv[]) {

   char c;
   while((int)(c = getchar()) != EOF){
        putchar(c);    
   }
	return 0;
}

int wc(int argc, char **argv) {

   char c;
	int lineCounter = 0;
	while ((int) (c = getchar()) != EOF)
		lineCounter += (c == '\n');
    printf("Number of lines: %d", lineCounter);
	return 0;
}


int filter(int argc, char **argv) {

   char c;
	while ((int) (c = getchar()) != EOF) {
		if (to_lower(c) == 'a' || to_lower(c) == 'e' || to_lower(c) == 'i' || to_lower(c) == 'o' || to_lower(c) == 'u')
			putchar(c);
	}
	putchar('\n');
	return 0;
}

size_t print_mem_info() {
    size_t *info = memory_info();
    printf("Total: %d, Used: %d, Free: %d", info[0], info[1], info[2]);
    return 0;
}
