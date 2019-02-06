/*
 * clibdocker: cld_command.c
 * Created on: 06-Feb-2019
 *
 * clibdocker
 * Copyright (C) 2018 Abhishek Mishra <abhishekmishra3@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include "cld_command.h"

/**
 * Create a new option given a name and type.
 *
 * \param option object to create
 * \param name
 * \param short_name
 * \param type
 * \param description
 * \return error code
 */
cld_command_error make_option(cld_option** option, char* name, char* short_name,
		cld_option_type type, char* description) {
	(*option) = (cld_option*) calloc(1, sizeof(cld_option));
	if ((*option) == NULL) {
		return CLD_COMMAND_ERR_ALLOC_FAILED;
	}
	(*option)->name = name;
	(*option)->short_name = short_name;
	(*option)->description = description;
	(*option)->bool_value = 0;
	(*option)->int_value = 0;
	(*option)->dbl_value = 0.0;
	(*option)->str_value = NULL;
	return CLD_COMMAND_SUCCESS;
}

/**
 * Free resources used by option
 */
void free_option(cld_option* option) {
	if (option->str_value) {
		free(option->str_value);
	}
	if (option->short_name) {
		free(option->short_name);
	}
	if (option->description) {
		free(option->description);
	}

	free(option->name);
	free(option);
}

/**
 * Create a new command with the given name and handler
 * Options and sub-commands need to be added after creation.
 * The sub-commands and options lists will be initialized,
 * so one just needs to add items using the arraylist add function.
 *
 * \param command obj to be created
 * \param name
 * \param short_name
 * \param description
 * \param handler function ptr to handler
 * \return error code
 */
cld_command_error make_command(cld_command** command, char* name,
		char* short_name, char* description, cld_command_handler handler) {
	(*command) = (cld_command*) calloc(1, sizeof(cld_command));
	if ((*command) == NULL) {
		return CLD_COMMAND_ERR_ALLOC_FAILED;
	}
	(*command)->name = name;
	(*command)->short_name = short_name;
	(*command)->description = description;
	(*command)->handler = handler;
	(*command)->options = array_list_new(&free_option);
	(*command)->sub_commands = array_list_new(&free_command);
	return CLD_COMMAND_SUCCESS;
}

/**
 * Free a command object
 */
void free_command(cld_command* command) {
	if (command->short_name) {
		free(command->short_name);
	}
	if (command->description) {
		free(command->description);
	}
	free(command->name);
	array_list_free(command->options);
	array_list_free(command->sub_commands);
	free(command);
}
