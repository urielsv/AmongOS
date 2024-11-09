/******************************************************************************
 *
 * @file    shell.h
 *
 * @brief   Shell.
 *
 * @author  Luca Pugliese                           <lpugliese@itba.edu.ar>
 * @author  Felipes Venturino                        <fventurino@itba.edu.ar>
 * @author  Uriel Sosa Vazquez                      <usosavazquez@itba.edu.ar>
 *
 ******************************************************************************/
#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <colors.h>
#include <definitions.h>

#define MAX_CMDS 2
#define MAX_ARGS 10
typedef struct command {
	char* name; 
	char* description;
	Function cmd;
} command_t;

typedef struct {
    char *cmd;
    char *argv[MAX_ARGS];
    int argc;
} command_input_t;

typedef struct {
    command_input_t cmds[MAX_CMDS];
    uint8_t cmd_count;
    uint8_t is_bg;
} parsed_input_t;

/*
 * @name shell
 * @brief Shell :).
 */
void shell();



int print_help();
int screen();
int print_clear();
int font();
int print_time();
int print_amongus();
int print_random();
int snake();
int print_regs();
int test_zero();
int opcode();

uint64_t test_mm_proc(uint64_t argc, char *argv[]);
uint64_t test_processes_proc(uint64_t argc, char *argv[]);
int test_prio_proc();
int test_synchro_proc(uint64_t argc, char *argv[]);

void kill_proc(int argc, char *argv[]);
void nice(int argc, char *argv[]);
void ps();
int cat(int argc, char **argv);
int wc(int argc, char **argv);
#endif
