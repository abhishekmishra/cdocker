/*
 * test_docker_containers.c
 *
 *  Created on: 16-Dec-2018
 *      Author: abhishek
 */
/* file minunit_example.c */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "test_docker_containers.h"
#include "docker_containers.h"
#include "docker_connection_util.h"

static docker_context* ctx = NULL;

static int group_setup(void **state) {
	char* id = NULL;
	docker_create_container_params* p;

	curl_global_init(CURL_GLOBAL_ALL);
	make_docker_context_socket(&ctx, "/var/run/docker.sock");
	make_docker_create_container_params(&p);
	p->image = "alpine";
	p->cmd = (char**) malloc(2 * sizeof(char*));
	p->cmd[0] = "echo";
	p->cmd[1] = "hello world";
	p->num_cmd = 2;
	docker_create_container(ctx, &id, p);
	assert_non_null(id);
	*state = id;
//	printf("id in state = %s\n", *state);
	return 0;
}
static int group_teardown(void **state) {
	curl_global_cleanup();
	free(*state);
	free_docker_context(&ctx);
	return 0;
}

static void test_start(void **state) {
	char* id = *state;
	docker_start_container(ctx, id);
	docker_wait_container(ctx, id);
	char* output;
	docker_stdout_container(ctx, &output, id);
	assert_non_null(output);
	assert_string_equal(output, "hello world\n");
}

static void test_list(void **state) {
	char* id = *state;
	docker_containers_list_filter* filter;
	make_docker_containers_list_filter(&filter);
	containers_filter_add_id(filter, id);
	docker_containers_list* containers;
	docker_container_list(ctx, &containers, 1, 5, 1, filter);
	printf("Read %d containers.\n", containers->num_containers);
	assert_int_equal(containers->num_containers, 1);
}

int docker_container_tests() {
	const struct CMUnitTest tests[] = {
	cmocka_unit_test(test_start),
	cmocka_unit_test(test_list) };
	return cmocka_run_group_tests_name("docker container tests", tests,
			group_setup, group_teardown);
}
