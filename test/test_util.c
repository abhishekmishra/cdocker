/**
 * Copyright (c) 2020 Abhishek Mishra
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "docker_all.h"
#include "docker_log.h"

void handle_error(docker_result* res) {
	docker_simple_error_handler_log(res);
}

void handle_result_for_test(docker_context* ctx, docker_result* res) {
	docker_log_info(res->url);
}
