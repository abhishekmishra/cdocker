/*
 * cld_common.c
 *
 *  Created on: 17-Feb-2019
 *      Author: abhis
 */

#include "cld_common.h"

docker_context* get_docker_context(void* handler_args) {
	docker_context* ctx = (docker_context*) handler_args;
	return ctx;
}

void handle_docker_error(docker_result* res) {
//	printf("\nURL: %s\n", get_docker_result_url(res));
	if (!is_ok(res)) {
		printf("DOCKER RESULT: Response error_code = %d, http_response = %ld\n",
				res->error_code, res->http_error_code);
		printf("error: %s\n", res->message);
	}
	free_docker_result(&res);
}
