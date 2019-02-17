/*
 * clibdocker: docker_cli.c
 * Created on: 20-Jan-2019
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

//based on example at http://www.cs.utah.edu/~bigler/code/libedit.html
//see https://stackoverflow.com/questions/36953960/is-editline-tab-completion-supposed-to-work
#if defined(__linux__) // can also use Linux here
#include <editline/readline.h>
#elif defined(__OpenBSD__)
#include <readline/readline.h>
#endif

#include <histedit.h>
#include <string.h>
#include <getopt.h>

#include "docker_all.h"
#include "cld_command.h"
#include "cld_sys.h"

#define CMD_NOT_FOUND -1

//cld_err_t cmd_hdlr_sys_connection(int argc, char** argv) {
//	if (ctx->socket) {
//		printf("Connected to unix socket: %s\n", ctx->socket);
//	}
//	if (ctx->url) {
//		printf("Connected to URL: %s\n", ctx->url);
//	}
//	return CLD_SUCCESS;
//}
//
//cld_cmd_t CLD_CMD_SYSTEM_VERSION = { "version", "ver", &cmd_hdlr_sys_version };
//cld_cmd_t CLD_CMD_SYSTEM_CONNECTION = { "connection", "conn",
//		&cmd_hdlr_sys_connection };
//cld_cmd_t* SYSTEM_COMMANDS[] = { &CLD_CMD_SYSTEM_VERSION,
//		&CLD_CMD_SYSTEM_CONNECTION, NULL };
//
//cld_err_t cmd_hdlr_system(int argc, char** argv) {
//	argc -= 1;
//	argv = &argv[1];
//	match_and_run_cmd(argc, argv, SYSTEM_COMMANDS);
//	return CLD_SUCCESS;
//}
//
//// END SYSTEM Commands
//
//// START CONTAINER Commands
//
//cld_err_t cmd_hdlr_container_list(int argc, char** argv) {
//	int all = 0;
//	int quiet = 0;
//	int c;
//	optind = 0;
//
//	while (1) {
//		static struct option container_ls_options[] = { { "filter",
//		required_argument, 0, 'f' }, { "all", no_argument, 0, 'a' }, { "quiet",
//		no_argument, 0, 'q' }, { 0, 0, 0, 0 } };
//
//		/* getopt_long stores the option index here. */
//		int option_index = 0;
//
//		c = getopt_long(argc, argv, "f:aq", container_ls_options,
//				&option_index);
//
//		/* Detect the end of the options. */
//		if (c == -1)
//			break;
//
//		switch (c) {
//		case 0:
//			/* If this option set a flag, do nothing else now. */
//			if (container_ls_options[option_index].flag != 0)
//				break;
//			printf("option %s", container_ls_options[option_index].name);
//			if (optarg)
//				printf(" with arg %s", optarg);
//			printf("\n");
//			break;
//
//		case 'a':
//			all = 1;
//			break;
//
//		case 'q':
//			quiet = 1;
//			break;
//
//		case 'f':
////			printf("option -f with value `%s'\n", optarg);
//			break;
//
//		case '?':
//			/* getopt_long already printed an error message. */
//			break;
//
//		default:
//			break;
//		}
//	}
//
//	if (optind < argc) {
//		printf("non-option ARGV-elements: ");
//		while (optind < argc)
//			printf("%s ", argv[optind++]);
//		putchar('\n');
//	}
//
//	docker_result* res;
//	docker_containers_list* containers;
//	docker_container_list(ctx, &res, &containers, all, 0, 1, NULL);
//	handle_docker_error(res);
//
//	if (quiet) {
//		for (int i = 0; i < docker_containers_list_length(containers); i++) {
//			docker_container_list_item* ctr = docker_containers_list_get_idx(
//					containers, i);
//			printf("%.*s\n", 12, ctr->id);
//		}
//	} else {
//
//		printf("%-20s\t%-40s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\n",
//				"CONTAINER ID", "IMAGE", "COMMAND", "CREATED", "STATUS",
//				"PORTS", "NAMES");
//		for (int i = 0; i < docker_containers_list_length(containers); i++) {
//			docker_container_list_item* ctr = docker_containers_list_get_idx(
//					containers, i);
//
//			//get ports
//			char ports_str[1024];
//			ports_str[0] = '\0';
//			for (int j = 0; j < array_list_length(ctr->ports); j++) {
//				char port_str[100];
//				docker_container_ports* ports = array_list_get_idx(ctr->ports,
//						0);
//				sprintf(port_str, "%ld:%ld", ports->private_port,
//						ports->public_port);
//				if (j == 0) {
//					strcpy(ports_str, port_str);
//				} else {
//					strcat(ports_str, ", ");
//					strcat(ports_str, port_str);
//				}
//			}
//
//			//get created time
//			time_t t = (time_t) ctr->created;
//			struct tm* timeinfo = localtime(&t);
//			char evt_time_str[256];
//			strftime(evt_time_str, 255, "%d-%m-%Y:%H:%M:%S", timeinfo);
//
//			//get names
//			char names[1024];
//			names[0] = '\0';
//			for (int j = 0; j < array_list_length(ctr->names); j++) {
//				if (j == 0) {
//					strcpy(names, array_list_get_idx(ctr->names, j));
//				} else {
//					strcat(names, ",");
//					strcat(names, array_list_get_idx(ctr->names, j));
//				}
//			}
//			printf("%-20.*s\t%-40s\t\"%-20s\"\t%-20s\t%-20s\t%-20s\t%-20s\t\n",
//					12, ctr->id, ctr->image, ctr->command, evt_time_str,
//					ctr->status, ports_str, names);
//		}
//	}
//	return CLD_SUCCESS;
//}
//
//cld_cmd_t CLD_CMD_CTR_LS = { "list", "ls", &cmd_hdlr_container_list };
//cld_cmd_t* CTR_COMMANDS[] = { &CLD_CMD_CTR_LS, NULL };
//
//cld_err_t cmd_hdlr_container(int argc, char** argv) {
//	argc -= 1;
//	argv = &argv[1];
//	match_and_run_cmd(argc, argv, CTR_COMMANDS);
//	return CLD_SUCCESS;
//}
//
//// END CONTAINER Commands
//
//cld_cmd_t CLD_CMD_SYSTEM = { "system", "sys", &cmd_hdlr_system };
//cld_cmd_t CLD_CMD_CONTAINER = { "container", "ctr", &cmd_hdlr_container };
//cld_cmd_t* TOPLEVEL_COMMANDS[] = { &CLD_CMD_SYSTEM, &CLD_CMD_CONTAINER, NULL };
//
//cld_err_t cmd_toplevel(int argc, char** argv) {
//	match_and_run_cmd(argc, argv, TOPLEVEL_COMMANDS);
//	return CLD_SUCCESS;
//}


struct array_list* create_commands() {
	struct array_list* commands = array_list_new(
			(void (*)(void *)) &free_command);
	array_list_add(commands, sys_commands());
	return commands;
}

char * prompt(EditLine *e) {
	return "cld> ";
}

int parse_line_run_command(Tokenizer* tokenizer, const char* line,
		int* cmd_argc, char*** cmd_argv, docker_context* ctx) {
	int tok_err = tok_str(tokenizer, line, &*cmd_argc,
			(const char***) &*cmd_argv);
	if (tok_err == 0) {
		cld_cmd_err err = exec_command(create_commands(), ctx, *cmd_argc,
				*cmd_argv);
		if (err != CLD_COMMAND_SUCCESS) {
			printf("Error: invalid command.\n");
		}
	} else {
		printf("Error: invalid command.\n");
	}

	//reset tokenizer;
	tok_reset(tokenizer);

	return tok_err;
}

int main(int argc, char *argv[]) {
	static docker_context* ctx;

	EditLine *el;
	History *myhistory;
	Tokenizer *tokenizer;
	int interactive = 1;
	int command = 0;
	char* command_str;
	char* url;

	/* Temp variables */
	int count;
	const char *line;
	int keepreading = 1;
	HistEvent ev;
	int cmd_argc;
	char** cmd_argv;

	/* Initialize the tokenizer */
	tokenizer = tok_init(NULL);

	/** No logging **/
	docker_log_set_quiet(1);

	/** Initialize docker context **/
	//make_docker_context_socket(&ctx, DOCKER_DEFINE_DEFAULT_UNIX_SOCKET);
	make_docker_context_url(&ctx, DOCKER_DEFINE_DEFAULT_UNIX_SOCKET);

	int c;

	while (1) {
		static struct option long_options[] = {
		/* These options don’t set a flag.
		 We distinguish them by their indices. */
		{ "command", required_argument, 0, 'c' }, { "interactive", no_argument,
				0, 'i' }, { 0, 0, 0, 0 } };
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long(argc, argv, "c:i", long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c) {
		case 0:
			/* If this option set a flag, do nothing else now. */
			if (long_options[option_index].flag != 0)
				break;
			printf("option %s", long_options[option_index].name);
			if (optarg)
				printf(" with arg %s", optarg);
			printf("\n");
			break;

		case 'i':
			puts("option -i\n");
			interactive = 2;
			break;

		case 'c':
//			printf("option -c with value `%s'\n", optarg);
			command = 1;
			command_str = optarg;
			break;

		case '?':
			/* getopt_long already printed an error message. */
			break;

		default:
			abort();
		}
	}

	/* Print any remaining command line arguments (not options). */
	int connected = 0;
	docker_result* res;
	if (optind < argc) {
		url = argv[optind];
		if (is_http_url(url)) {
			if (make_docker_context_url(&ctx, url) == E_SUCCESS) {
				connected = 1;
			}
		} else if (is_unix_socket(url)) {
			if (make_docker_context_socket(&ctx, url) == E_SUCCESS) {
				connected = 1;
			}
		}
		optind++;

		if (optind < argc) {
			printf("There are extra arguments: ");
			while (optind < argc)
				printf("%s ", argv[optind++]);
			putchar('\n');
		}

	} else {
		url = DOCKER_DEFINE_DEFAULT_UNIX_SOCKET;
		if (make_docker_context_socket(&ctx, url) == E_SUCCESS) {
			connected = 1;
		}
	}

	if (connected) {
		if (docker_ping(ctx, &res) != E_SUCCESS) {
			docker_log_fatal("Could not ping the server %s", url);
			connected = 0;
		} else {
			docker_log_info("%s is alive.", url);
		}
	}

	if (command) {
		int tok_err = parse_line_run_command(tokenizer, command_str, &cmd_argc,
				&cmd_argv, ctx);
	}

	if (interactive > command) {
		printf("=========================================\n");
		printf("== CLD (clibdocker) Docker CLI.        ==\n");
		printf("== Version: 0.1a                       ==\n");
		printf("==                                     ==\n");
		printf("== For help type help <command>        ==\n");
		printf("=========================================\n");

		/* Initialize the EditLine state to use our prompt function and
		 emacs style editing. */
		el = el_init(argv[0], stdin, stdout, stderr);
		el_set(el, EL_PROMPT, &prompt);
		el_set(el, EL_EDITOR, "emacs");

		/* Initialize the history */
		myhistory = history_init();
		if (myhistory == 0) {
			fprintf(stderr, "history could not be initialized\n");
			return 1;
		}

		/* Set the size of the history */
		history(myhistory, &ev, H_SETSIZE, 800);

		/* This sets up the call back functions for history functionality */
		el_set(el, EL_HIST, history, myhistory);

		while (keepreading) {
			/* count is the number of characters read.
			 line is a const char* of our command line with the tailing \n */
			line = el_gets(el, &count);
			if (line != NULL) {
				if (strcmp("quit\n", line) == 0 || strcmp("q\n", line) == 0) {
					printf("Ending session.\n");
					el_beep(el);
					goto CLEANUP_AND_EXIT;
				}
				int tok_err = parse_line_run_command(tokenizer, line, &cmd_argc,
						&cmd_argv, ctx);
			}

			/* In order to use our history we have to explicitly add commands
			 to the history */
			if (count > 0) {
				history(myhistory, &ev, H_ENTER, line);
//			printf("You typed \"%s\"\n", line);
			}
		}
		CLEANUP_AND_EXIT:
		/* Clean up our memory */
		history_end(myhistory);
		el_end(el);
	}

	tok_end(tokenizer);
	return 0;
}
