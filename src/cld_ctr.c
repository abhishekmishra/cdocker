#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cld_ctr.h"
#include "docker_all.h"
#include "cld_table.h"

cld_cmd_err ctr_ls_cmd_handler(void *handler_args, struct array_list *options,
		struct array_list *args, cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_result *res;
	docker_context *ctx = get_docker_context(handler_args);
	docker_containers_list *containers;
	docker_container_list(ctx, &res, &containers, 1, 0, 1, NULL);
	handle_docker_error(res, success_handler, error_handler);

	if (quiet) {
		for (int i = 0; i < docker_containers_list_length(containers); i++) {
			docker_container_list_item *ctr = docker_containers_list_get_idx(
					containers, i);
			printf("%.*s\n", 12, ctr->id);
		}
	} else {
		int num_containers = docker_containers_list_length(containers);
		cld_table* ctr_tbl;
		if (create_cld_table(&ctr_tbl, num_containers, 7) == 0) {
			cld_table_set_header(ctr_tbl, 0, "CONTAINER ID");
			cld_table_set_header(ctr_tbl, 1, "IMAGE");
			cld_table_set_header(ctr_tbl, 2, "COMMAND");
			cld_table_set_header(ctr_tbl, 3, "CREATED");
			cld_table_set_header(ctr_tbl, 4, "STATUS");
			cld_table_set_header(ctr_tbl, 5, "PORTS");
			cld_table_set_header(ctr_tbl, 6, "NAMES");

			for (int i = 0; i < docker_containers_list_length(containers);
					i++) {
				docker_container_list_item *ctr =
						docker_containers_list_get_idx(containers, i);

				//get ports
				char ports_str[1024];
				ports_str[0] = '\0';
				for (int j = 0; j < array_list_length(ctr->ports); j++) {
					char port_str[100];
					docker_container_ports *ports = array_list_get_idx(
							ctr->ports, 0);
					sprintf(port_str, "%ld:%ld", ports->private_port,
							ports->public_port);
					if (j == 0) {
						strcpy(ports_str, port_str);
					} else {
						strcat(ports_str, ", ");
						strcat(ports_str, port_str);
					}
				}

				//get created time
				time_t t = (time_t) ctr->created;
				struct tm *timeinfo = localtime(&t);
				char evt_time_str[256];
				strftime(evt_time_str, 255, "%d-%m-%Y:%H:%M:%S", timeinfo);

				//get names
				char names[1024];
				names[0] = '\0';
				for (int j = 0; j < array_list_length(ctr->names); j++) {
					if (j == 0) {
						strcpy(names, array_list_get_idx(ctr->names, j));
					} else {
						strcat(names, ",");
						strcat(names, array_list_get_idx(ctr->names, j));
					}
				}
				cld_table_set_row_val(ctr_tbl, i, 0, ctr->id);
				cld_table_set_row_val(ctr_tbl, i, 1, ctr->image);
				cld_table_set_row_val(ctr_tbl, i, 2, ctr->command);
				cld_table_set_row_val(ctr_tbl, i, 3, evt_time_str);
				cld_table_set_row_val(ctr_tbl, i, 4, ctr->status);
				cld_table_set_row_val(ctr_tbl, i, 5, ports_str);
				cld_table_set_row_val(ctr_tbl, i, 6, names);
			}
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_TABLE, ctr_tbl);
			free_cld_table(ctr_tbl);
		}
	}

	array_list_free(containers);
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_create_cmd_handler(void *handler_args,
		struct array_list *options, struct array_list *args,
		cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_result *res;
	docker_context *ctx = get_docker_context(handler_args);
	int len = array_list_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
				"Image name not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	} else {
		cld_argument* image_name_arg = (cld_argument*) array_list_get_idx(args,
				0);
		char* image_name = image_name_arg->val->str_value;
		char* id = NULL;
		docker_create_container_params* p;
		make_docker_create_container_params(&p);
		p->image = image_name;
		docker_create_container(ctx, &res, &id, p);
		int created = is_ok(res);
		handle_docker_error(res, success_handler, error_handler);
		if (created) {
			if (id != NULL) {
				char res_str[1024];
				sprintf(res_str, "Created container with id %s", id);
				success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING,
						res_str);
			}
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_command *ctr_commands() {
	cld_command *container_command;
	if (make_command(&container_command, "container", "ctr",
			"Docker Container Commands",
			NULL) == CLD_COMMAND_SUCCESS) {
		cld_command *ctrls_command, *ctrcreate_command;
		if (make_command(&ctrls_command, "list", "ls", "Docker Container List",
				&ctr_ls_cmd_handler) == CLD_COMMAND_SUCCESS) {
			array_list_add(container_command->sub_commands, ctrls_command);
		}
		if (make_command(&ctrcreate_command, "create", "create",
				"Docker Container Create", &ctr_create_cmd_handler)
				== CLD_COMMAND_SUCCESS) {
			cld_argument* image_name_arg;
			make_argument(&image_name_arg, "Image Name", CLD_TYPE_STRING,
					"Name of Docker Image to use.");
			array_list_add(ctrcreate_command->args, image_name_arg);

			array_list_add(container_command->sub_commands, ctrcreate_command);
		}
	}
	return container_command;
}
