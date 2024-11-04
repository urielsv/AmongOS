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

#include <definitions.h>

typedef struct command {
	char* name;
	char* description;
	Function cmd;
} command_t;

#endif