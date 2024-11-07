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


/*
 * @name shell
 * @brief Shell :).
 */
void shell();


int execute_command(char *cmd, char **argv, int argc);

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
#endif
