/**
 * Copyright (c) 2020 Abhishek Mishra
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>

#include "docker_log.h"
#include "test_docker_containers.h"
#include "test_docker_system.h"
#include "test_docker_images.h"
#include "test_docker_networks.h"
#include "test_docker_volumes.h"
#include <docker_result.h>
#include <docker_containers.h>

int main(int argc, char **argv) {
	int res = 0;
	docker_log_set_level(LOG_INFO);
	docker_log_info("#### Starting clibdocker tests ####");

	docker_log_info("#### Docker container API      ####");

	//d_err_t e;
	//static docker_context* ctx = NULL;
	//curl_global_init(CURL_GLOBAL_ALL);
	//make_docker_context_default_local(&ctx);

	//char* id;
	//docker_ctr_create_params* p = make_docker_ctr_create_params();
	////docker_ctr_create_params_image_set(p, "bfirsh/reticulate-splines");
	//docker_ctr_create_params_image_set(p, "alpine");
	//docker_ctr_create_params_entrypoint_set(p, "sh");
	//docker_ctr_create_params_attachstdin_set(p, 1);
	//docker_ctr_create_params_tty_set(p, 1);
	//docker_ctr_create_params_openstdin_set(p, 1);
	//e = docker_create_container(ctx, &id, p);
	//assert_int_equal(e, E_SUCCESS);
	//free_docker_ctr_create_params(p);

	//e = docker_start_container(ctx, id, NULL);
	//assert_int_equal(e, E_SUCCESS);

	//docker_log_info("Started docker container id is %s\n", id);

	//e = docker_container_attach_default(ctx, id, NULL, 1, 1, 1, 1, 1);
	//assert_int_equal(e, E_SUCCESS);

	//e = docker_stop_container(ctx, id, 0);
	//assert_int_equal(e, E_SUCCESS);

	//e = docker_remove_container(ctx, id, 0, 0, 0);
	//assert_int_equal(e, E_SUCCESS);

	res = docker_container_tests();
	docker_log_info("#### Done                      ####");

	if (res > 0) {
		return res;
	}

	//docker_log_info("#### Docker images API      	####");
	//res = docker_images_tests();
	//docker_log_info("#### Done                      ####");

	//if (res > 0) {
	//	return res;
	//}

	//docker_log_info("#### Docker system API      	####");
	//res = docker_system_tests();
	//docker_log_info("#### Done                      ####");

	//if (res > 0) {
	//	return res;
	//}

	//docker_log_info("#### Docker networks API      	####");
	//res = docker_networks_tests();
	//docker_log_info("#### Done                      ####");

	//if (res > 0) {
	//	return res;
	//}

	//docker_log_info("#### Docker volumes API      	####");
	//res = docker_volumes_tests();
	//docker_log_info("#### Done                      ####");

	return res;
}

