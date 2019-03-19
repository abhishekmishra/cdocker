/*
 * cld_sys.c
 *
 *  Created on: 17-Feb-2019
 *      Author: abhis
 */

#include "cld_sys.h"
#include "docker_all.h"
#include "cld_dict.h"

cld_cmd_err sys_version_cmd_handler(void* handler_args,
		struct array_list* options, struct array_list* args,
		cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	docker_result* res;
	docker_version* version;
	docker_context* ctx = get_docker_context(handler_args);
	docker_system_version(ctx, &res, &version);
	handle_docker_error(res);

	cld_dict* ver_dict;
	if (create_cld_dict(&ver_dict) == 0) {
		cld_dict_put(ver_dict, "Docker Version", version->version);
		cld_dict_put(ver_dict, "OS", version->os);
		cld_dict_put(ver_dict, "Kernel", version->kernel_version);
		cld_dict_put(ver_dict, "Arch", version->arch);
		cld_dict_put(ver_dict, "API Version", version->api_version);
		cld_dict_put(ver_dict, "Min API Version", version->min_api_version);
		cld_dict_put(ver_dict, "Go Version", version->go_version);
		cld_dict_put(ver_dict, "Git Commit", version->git_commit);
		cld_dict_put(ver_dict, "Build Time", version->build_time);
		cld_dict_put(ver_dict, "Experimental",
				version->experimental == 0 ? "False" : "True");

		success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_DICT, ver_dict);

		free_cld_dict(ver_dict);
	}

	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err sys_connection_cmd_handler(void* handler_args,
		struct array_list* options, struct array_list* args,
		cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	docker_context* ctx = get_docker_context(handler_args);
	if (ctx->socket) {
		printf("Connected to unix socket: %s\n", ctx->socket);
	}
	if (ctx->url) {
		printf("Connected to URL: %s\n", ctx->url);
	}
	return CLD_COMMAND_SUCCESS;
}

cld_command* sys_commands() {
	cld_command* system_command;
	if (make_command(&system_command, "system", "sys", "Docker System Commands",
	NULL) == CLD_COMMAND_SUCCESS) {
		cld_command *sysver_command, *syscon_command;
		if (make_command(&sysver_command, "version", "ver",
				"Docker System Version", &sys_version_cmd_handler)
				== CLD_COMMAND_SUCCESS) {
			array_list_add(system_command->sub_commands, sysver_command);
		}
		if (make_command(&syscon_command, "connection", "con",
				"Docker System Connection", &sys_connection_cmd_handler)
				== CLD_COMMAND_SUCCESS) {
			array_list_add(system_command->sub_commands, syscon_command);
		}
	}
	return system_command;
}

