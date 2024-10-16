/******************************************************************************
 *
 * @file    commands.h
 *
 * @brief   Shell commands.
 *
 * @author  Luca Pugliese                           <lpugliese@itba.edu.ar>
 * @author  Felipes Venturino                        <fventurino@itba.edu.ar>
 * @author  Uriel Sosa Vazquez                      <usosavazquez@itba.edu.ar>
 *
 ******************************************************************************/
#ifndef COMMANDS_H
#define COMMANDS_H

typedef struct command {
	char* name;
	char* description;
	int (*cmd)();
} command_t;

int execute_command(char* cmdname);

// Commands
int print_help();
int screen();
int font();
int print_amongus();
int print_random();
int print_time();
int print_clear();
int print_regs();
int testzero();
int opcode();

#endif