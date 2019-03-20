#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cld_img.h"
#include "docker_all.h"

typedef struct {
	cld_command_output_handler success_handler;
	array_list* id_ls;
	array_list* progress_ls;
	array_list* message_ls;
//	array_list* current_ls;
//	array_list* total_ls;
} docker_pull_update_args;

void log_pull_message(docker_image_create_status* status, void* client_cbargs) {
	docker_pull_update_args* upd_args = (docker_pull_update_args*) client_cbargs;
	char* res_str = (char*) calloc(1024, sizeof(char));
	if (status) {
		if (status->id) {
			sprintf(res_str, "message is %s, id is %s", status->status,
					status->id);
			int len = array_list_length(upd_args->id_ls);
			int new_len = len;
			int found = 0;
			int loc = -1;
			for (int i = 0; i < len; i++) {
				if (strcmp(status->id,
						(char*) array_list_get_idx(upd_args->id_ls, i)) == 0) {
					found = 1;
					loc = i;
				}
			}
			if (found == 0) {
				array_list_add(upd_args->id_ls, status->id);
				array_list_add(upd_args->message_ls, status->status);
				new_len += 1;
				if (status->progress != NULL) {
					array_list_add(upd_args->progress_ls, status->progress);
//					array_list_add(upd_args->current_ls,
//							&(status->progress_detail->current));
//					array_list_add(upd_args->total_ls,
//							&(status->progress_detail->total));
				}
			} else {
				array_list_put_idx(upd_args->id_ls, loc, status->id);
				array_list_put_idx(upd_args->message_ls, loc, status->status);

				if (status->progress != NULL) {
					array_list_put_idx(upd_args->progress_ls, loc,
							status->progress);
				}
			}
			//printf("lines to clear %d, lines to write %d\n", len, new_len);
			printf("\033[%dA", len);
			fflush(stdout);
			for (int i = 0; i < new_len; i++) {
				printf("\033[K%s: %s",
						(char*) array_list_get_idx(upd_args->id_ls, i),
						(char*) array_list_get_idx(upd_args->message_ls, i));
				char* progress = (char*) array_list_get_idx(
						upd_args->progress_ls, i);
				if (progress != NULL) {
					printf(" %s", progress);
				}
				printf("\n");
			}
//			if (status->progress != NULL) {
//				printf("%s\n", status->progress);
//			}
		} else {
			sprintf(res_str, "message is %s", status->status);
		}
//		if (upd_args->success_handler) {
//			upd_args->success_handler(CLD_COMMAND_IS_RUNNING, CLD_RESULT_STRING, res_str);
//		}
	}
	free(res_str);
}

cld_cmd_err img_pl_cmd_handler(void *handler_args, struct array_list *options,
		struct array_list *args, cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_result *res;
	docker_context *ctx = get_docker_context(handler_args);

	docker_pull_update_args* upd_args = (docker_pull_update_args*) calloc(1,
			sizeof(docker_pull_update_args));
	upd_args->success_handler = success_handler;
	upd_args->id_ls = array_list_new(&free);
	upd_args->message_ls = array_list_new(&free);
//	upd_args->current_ls = array_list_new(&free);
//	upd_args->total_ls = array_list_new(&free);
	upd_args->progress_ls = array_list_new(&free);

	int len = array_list_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
				"Image name not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	} else {
		cld_argument* image_name_arg = (cld_argument*) array_list_get_idx(args,
				0);
		char* image_name = image_name_arg->val->str_value;
		d_err_t docker_error = docker_image_create_from_image_cb(ctx, &res,
				&log_pull_message, upd_args, image_name, NULL, NULL);
		handle_docker_error(res);
		if (docker_error == E_SUCCESS) {
			char* res_str = (char*) calloc(1024, sizeof(char));
			sprintf(res_str, "Image pull successful -> %s", image_name);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
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
