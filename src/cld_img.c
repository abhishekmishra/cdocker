#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cld_img.h"
#include "docker_all.h"

void log_pull_message(docker_image_create_status* status, void* client_cbargs) {
	cld_command_output_handler success_handler = (cld_command_output_handler) client_cbargs;
	char* res_str = (char*) calloc(1024, sizeof(char));
	if (status) {
		if (status->id) {
			sprintf(res_str, "message is %s, id is %s", status->status,
					status->id);
		} else {
			sprintf(res_str, "message is %s", status->status);
		}
		if(success_handler) {
			success_handler(CLD_RESULT_STRING, res_str, CLD_COMMAND_IS_RUNNING);
		}
	}
	free(res_str);
}

cld_cmd_err img_pl_cmd_handler(void *handler_args, struct array_list *options,
		struct array_list *args, cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_result *res;
	docker_context *ctx = get_docker_context(handler_args);

	int len = array_list_length(args);
	if (len != 1) {
		error_handler(CLD_RESULT_STRING, "Image name not provided.",
				CLD_COMMAND_ERR_UNKNOWN);
		return CLD_COMMAND_ERR_UNKNOWN;
	} else {
		cld_argument* image_name_arg = (cld_argument*) array_list_get_idx(args,
				0);
		char* image_name = image_name_arg->val->str_value;
		d_err_t docker_error = docker_image_create_from_image_cb(ctx, &res,
				&log_pull_message, success_handler, image_name, NULL, NULL);
		handle_docker_error(res);
		if (docker_error == E_SUCCESS) {
			char* res_str = (char*) calloc(1024, sizeof(char));
			sprintf(res_str, "Image pull successful -> %s", image_name);
			success_handler(CLD_RESULT_STRING, res_str, CLD_COMMAND_SUCCESS);
			free(res_str);
			return CLD_COMMAND_SUCCESS;
		} else {
			return CLD_COMMAND_ERR_UNKNOWN;
		}
	}
}

cld_command *img_commands() {
	cld_command *image_command;
	if (make_command(&image_command, "image", "img", "Docker Image Commands",
	NULL) == CLD_COMMAND_SUCCESS) {
		cld_command *imgpl_command;
		if (make_command(&imgpl_command, "pull", "pl", "Docker Image Pull",
				&img_pl_cmd_handler) == CLD_COMMAND_SUCCESS) {
			cld_argument* image_name_arg;
			make_argument(&image_name_arg, "Image Name", CLD_TYPE_STRING,
					"Name of Docker Image to be pulled.");
			array_list_add(imgpl_command->args, image_name_arg);

			array_list_add(image_command->sub_commands, imgpl_command);
		}
	}
	return image_command;
}
