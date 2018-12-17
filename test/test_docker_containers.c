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

static void null_test_success(void **state) {
	curl_global_init(CURL_GLOBAL_ALL);

    char* id;
    docker_create_container_params* p = make_docker_create_container_params();
    p->image = "alpine";
    p->cmd = (char**)malloc(2 * sizeof(char*));
    p->cmd[0] = "echo";
    p->cmd[1] = "hello world";
    p->num_cmd = 2;
    id = docker_create_container(p);

    printf("Docker container id is %s\n", id);

    docker_start_container(id);
    docker_wait_container(id);
    docker_stdout_container(id);

	printf("\n\n========== Docker containers list.=========\n");

	docker_containers_list_filter* filter = make_docker_containers_list_filter();
	//containers_filter_add_name(filter, "/registryui");
	containers_filter_add_id(filter, "7460166eeca46468a217d3fb058924f07bebb674a3502fa1b0440cd933cfc9ff");
	docker_containers_list* containers = docker_container_list(1, 5, 1, filter);
	printf("Read %d containers.\n", containers->num_containers);

	curl_global_cleanup();
}

int docker_container_tests() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(null_test_success),
    };
    return cmocka_run_group_tests_name("docker container tests", tests, NULL, NULL);
}
