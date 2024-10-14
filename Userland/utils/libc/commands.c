// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <commands.h>
#include <random.h>
#include <snake.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/string.h"
#include <syscalls.h>

static command_t commands[] = {
    {"help", "Shows this help", print_help},
    {"clear", "Clears the screen", print_clear},
    {"time", "Shows the current time", print_time},
    {"random", "Shows a random number", print_random},
    {"screen", "Shows the screen size", screen},
    {"font", "Change font size (1-5)", font},
    {"exit", "Exits the shell", NULL},
    {"amongus", "Prints Among Us", print_amongus},
    {"snake", "Starts the snake game", snake},
    {"regs", "printea los registros", print_regs},
    {"zero", "testea la excepcion de division por cero", testzero},
    {"opcode", "testea la excepcion de opcode invalido", opcode},	
};

void execute_command(char *cmdname) {
    for (int i = 0; i < sizeof(commands) / sizeof(command_t); i++) {
        if (strcmp(commands[i].name, cmdname) == 0) {
            commands[i].cmd();
            return;
        }
    }
    printf("\nSUShell: '%s' command not found\n", cmdname);
}
void print_help() {

    printf("\nCommand: Description\n");
    for (int i = 1; i < sizeof(commands) / sizeof(command_t); i++) {
        printf("%s: %s\n", commands[i].name, commands[i].description);
    }
}

void screen() {
    uint64_t width, height;
    screen_info(&width, &height);
    printf("\nW:%d, H:%d\n", width, height);
}

void font() {
    int num;
    scanf("\nNew size: %d", &num);
    if (num <= 0 || num >= 6) {
        printf("\nInvalid size\n");
    } else {
        font_size(num);
        clear(COLOR_BLACK);
    }
}

void print_amongus() {
    printf_color("\n........ooooooooo.......\n", COLOR_RED, COLOR_BLACK);
    printf_color("......oo.....oooooo.....\n", COLOR_RED, COLOR_BLACK);
    printf_color("......oo...oo......oo...\n", COLOR_RED, COLOR_BLACK);
    printf_color("......oo.....oooooo.....\n", COLOR_RED, COLOR_BLACK);
    printf_color("......oo.......oo.......\n", COLOR_RED, COLOR_BLACK);
    printf_color("......oo...oo..oo.......\n", COLOR_RED, COLOR_BLACK);
    printf_color("......oo...oo..oo.......\n", COLOR_RED, COLOR_BLACK);
    printf_color("......ooo..oo.ooo.......\n", COLOR_RED, COLOR_BLACK);
}

void print_random() {
    printf("\n%d\n", random(1203929));
}

void print_time() {
    printf("\n%s\n", time());
}

void print_clear() {
    clear(COLOR_BLACK);
}

void print_regs() {
    sys_registers();
}

void testzero(){
    test_exc_zero();
}

void opcode(){
    test_exc_invalid_opcode();
}