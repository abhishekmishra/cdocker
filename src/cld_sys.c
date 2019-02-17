/*
 * cld_sys.c
 *
 *  Created on: 17-Feb-2019
 *      Author: abhis
 */

#include "cld_common.h"
#include "cld_command.h"
#include "docker_all.h"

cld_cmd_err sys_version_cmd_handler(void* handler_args,
		struct array_list* options, struct array_list* args,
		cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	printf("Print version now\n");
	docker_result* res;
	docker_version* version;
	docker_context* ctx = get_docker_context(handler_args);
	docker_system_version(ctx, &res, &version);
	handle_docker_error(res);

	printf("\n"
			"-- Docker Server Version Info\n"
			"Docker Version:  %s\n"
			"OS (Kernel):     %s (%s)\n"
			"Arch:            %s\n"
			"API Version:     %s\n"
			"Min API Version: %s\n"
			"Go Version:      %s\n"
			"Git Commit:      %s\n"
			"Build Time:      %s\n"
			"Experimental:    %d"
			"\n--\n", version->version, version->os, version->kernel_version,
			version->arch, version->api_version, version->min_api_version,
			version->go_version, version->git_commit, version->build_time,
			version->experimental);

	return CLD_COMMAND_SUCCESS;
}

cld_command* sys_commands() {
	cld_command* system_command;
	cld_cmd_err err = make_command(&system_command, "system", "sys",
			"Docker System Commands", NULL);
	if (err == CLD_COMMAND_SUCCESS) {
		cld_command* sysver_command;
		err = make_command(&sysver_command, "version", "ver",
				"Docker System Version", &sys_version_cmd_handler);
		if (err == CLD_COMMAND_SUCCESS) {
			array_list_add(system_command->sub_commands, sysver_command);
		}
	}
	return system_command;
}

