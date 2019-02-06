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
 * Create a new value object of given type.
 *
 * \param val object to create
 * \param type
 * \return error code
 */
cld_cmd_err make_cld_val(cld_val** val, cld_type type) {
	(*val) = (cld_val*) calloc(1, sizeof(cld_val));
	if ((*val) == NULL) {
		return CLD_COMMAND_ERR_ALLOC_FAILED;
	}
	(*val)->type = type;
	clear_cld_val(*val);
	return CLD_COMMAND_SUCCESS;
}

/**
 * Free the created value
 */
void free_cld_val(cld_val* val) {
	if (val->str_value) {
		free(val->str_value);
	}
	free(val);
}

/**
 * Reset values to system defaults.
 */
void clear_cld_val(cld_val* val) {
	val->bool_value = 0;
	val->int_value = 0;
	val->dbl_value = 0.0;
	val->str_value = NULL;
}

/**
 * Parse the input and read the value of the type of the val object.
 * (Should not be called when the values is a flag.)
 * The value should be set as soon as the argument/option is seen
 *
 * \param val object whose value will be set
 * \param input string input
 * \return error code
 */
cld_cmd_err parse_cld_val(cld_val* val, char* input) {
	if (input == NULL) {
		return CLD_COMMAND_ERR_UNKNOWN;
	} else {
		int v;
		double d;
		switch (val->type) {
		case CLD_TYPE_BOOLEAN:
			sscanf(input, "%d", &v);
			if (v > 0) {
				val->bool_value = 1;
			} else {
				val->bool_value = 0;
			}
			break;
		case CLD_TYPE_INT:
			sscanf(input, "%d", &v);
			val->int_value = v;
			break;
		case CLD_TYPE_DOUBLE:
			sscanf(input, "%g", &d);
			val->dbl_value = d;
			break;
		case CLD_TYPE_STRING:
			val->str_value = input;
			break;
		default:
			return CLD_COMMAND_ERR_UNKNOWN;
		}
		return CLD_COMMAND_SUCCESS;
	}
}

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
cld_cmd_err make_option(cld_option** option, char* name, char* short_name,
		cld_type type, char* description) {
	(*option) = (cld_option*) calloc(1, sizeof(cld_option));
	if ((*option) == NULL) {
		return CLD_COMMAND_ERR_ALLOC_FAILED;
	}
	(*option)->name = name;
	(*option)->short_name = short_name;
	(*option)->description = description;
	make_cld_val(&((*option)->val), type);
	return CLD_COMMAND_SUCCESS;
}

/**
 * Free resources used by option
 */
void free_option(cld_option* option) {
	if (option->short_name) {
		free(option->short_name);
	}
	if (option->description) {
		free(option->description);
	}

	free(option->val);
	free(option->name);
	free(option);
}

/**
 * Create a new argument given a name and type.
 *
 * \param argument object to create
 * \param name
 * \param type
 * \param description
 * \return error code
 */
cld_cmd_err make_argument(cld_argument** arg, char* name, cld_type type,
		char* description) {
	(*arg) = (cld_argument*) calloc(1, sizeof(cld_argument));
	if ((*arg) == NULL) {
		return CLD_COMMAND_ERR_ALLOC_FAILED;
	}
	(*arg)->name = name;
	(*arg)->description = description;
	(*arg)->optional = 0;
	make_cld_val(&((*arg)->val), type);
	return CLD_COMMAND_SUCCESS;
}

/**
 * Free resources used by argument
 */
void free_argument(cld_argument* arg) {
	if (arg->description) {
		free(arg->description);
	}
	free(arg->val);
	free(arg->name);
	free(arg);
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
cld_cmd_err make_command(cld_command** command, char* name, char* short_name,
		char* description, cld_command_handler handler) {
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
	(*command)->args = array_list_new(&free_argument);
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
	array_list_free(command->args);
	free(command);
}
