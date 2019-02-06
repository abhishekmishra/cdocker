/*
 * clibdocker: cld_command.h
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

#ifndef SRC_CLD_COMMAND_H_
#define SRC_CLD_COMMAND_H_

#include "docker_connection_util.h"
#include <json-c/arraylist.h>

typedef enum {
	CLD_COMMAND_SUCCESS = 0,
	CLD_COMMAND_ERR_UNKNOWN = 1,
	CLD_COMMAND_ERR_ALLOC_FAILED = 2
} cld_command_error;

typedef enum {
	CLD_OPTION_BOOLEAN = 0,
	CLD_OPTION_INT = 1,
	CLD_OPTION_DOUBLE = 2,
	CLD_OPTION_STRING = 3
} cld_option_type;

typedef struct cld_option_t {
	char* name;
	char* short_name;
	cld_option_type type;
	int bool_value;
	int int_value;
	double dbl_value;
	char* str_value;
	char* description;
} cld_option;

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
		cld_option_type type, char* description);

/**
 * Free resources used by option
 */
void free_option(cld_option* option);

typedef cld_command_error (*cld_command_output_handler)(char* result,
		cld_command_error result_flag);
typedef cld_command_error (*cld_command_handler)(docker_context* ctx,
		struct array_list* options, cld_command_output_handler success_handler,
		cld_command_output_handler error_handler);

typedef struct cld_command_t {
	char* name;
	char* short_name;
	char* description;
	struct array_list* sub_commands;
	struct array_list* options;
	cld_command_handler handler;
} cld_command;

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
		char* short_name, char* description, cld_command_handler handler);

/**
 * Free a command object
 */
void free_command(cld_command* command);

#endif /* SRC_CLD_COMMAND_H_ */
