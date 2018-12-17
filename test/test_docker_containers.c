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

static int group_setup(void **state) {
	curl_global_init(CURL_GLOBAL_ALL);
	char* id = NULL;
	docker_create_container_params* p = make_docker_create_container_params();
	p->image = "alpine";
	p->cmd = (char**) malloc(2 * sizeof(char*));
	p->cmd[0] = "echo";
	p->cmd[1] = "hello world";
	p->num_cmd = 2;
	id = docker_create_container(p);
	assert_non_null(id);
	*state = id;
	printf("id in state = %s\n", *state);
	return 0;
}
static int group_teardown(void **state) {
	curl_global_cleanup();
	free(*state);
	return 0;
}

//static void test_create(void **state) {
//	char* id = NULL;
//	docker_create_container_params* p = make_docker_create_container_params();
//	p->image = "alpine";
//	p->cmd = (char**) malloc(2 * sizeof(char*));
//	p->cmd[0] = "echo";
//	p->cmd[1] = "hello world";
//	p->num_cmd = 2;
//	id = docker_create_container(p);
//	assert_non_null(id);
//	*state = id;
//	printf("id in state = %s\n", *state);
//}

static void test_start(void **state) {
	printf("id in state = %s\n", *state);
	char* id = *state;

	printf("Docker container id is %s\n", id);

	docker_start_container(id);
	docker_wait_container(id);
	char* output = docker_stdout_container(id);
	assert_non_null(output);
	assert_string_equal(output, "hello world\n");

//	printf("\n\n========== Docker containers list.=========\n");
//
//	docker_containers_list_filter* filter =
//			make_docker_containers_list_filter();
//	//containers_filter_add_name(filter, "/registryui");
//	containers_filter_add_id(filter,
//			"7460166eeca46468a217d3fb058924f07bebb674a3502fa1b0440cd933cfc9ff");
//	docker_containers_list* containers = docker_container_list(1, 5, 1, filter);
//	printf("Read %d containers.\n", containers->num_containers);
}

int docker_container_tests() {
	const struct CMUnitTest tests[] = {
	cmocka_unit_test(test_start) };
	return cmocka_run_group_tests_name("docker container tests", tests,
			group_setup, group_teardown);
}
