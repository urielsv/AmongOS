/******************************************************************************
 *
 * @file    shell.h
 *
 * @brief   shell.
 *
 * @author  luca pugliese                           <lpugliese@itba.edu.ar>
 * @author  felipes venturino                        <fventurino@itba.edu.ar>
 * @author  uriel sosa vazquez                      <usosavazquez@itba.edu.ar>
 *
 ******************************************************************************/
#ifndef shell_h
#define shell_h

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <colors.h>
#include <definitions.h>
#include <stddef.h>

#define max_cmds 2
#define max_args 10
typedef struct command {
	char* name; 
	char* description;
	function cmd;
} command_t;

typedef struct {
    char *cmd;
    char *argv[max_args];
    int argc;
} command_input_t;

typedef struct {
    command_input_t cmds[max_cmds];
    uint8_t cmd_count;
    uint8_t is_bg;
} parsed_input_t;

/*
 * @name shell
 * @brief shell :).
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
int philos_proc(int argc, char *argv[]);
void block_proc(int argc, char *argv[]);
void unblock_proc(int argc, char *argv[]);

void kill_proc(int argc, char *argv[]);
void nice(int argc, char *argv[]);
void ps();
int cat(int argc, char **argv);
int wc(int argc, char **argv);
int filter(int argc, char **argv);
size_t print_mem_info();
#endif
