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
#include <stdlib.h>
#include <curl/curl.h>

#include "test_docker_networks.h"

#include "docker_log.h"
#include "docker_networks.h"
#include "docker_connection_util.h"
#include "test_util.h"

#include <json-c/arraylist.h>

static docker_context* ctx = NULL;
static int http_response_code = 0;

void handle_result_net(docker_context* ctx, docker_result* res) {
	http_response_code = docker_result_get_http_error_code(res);
}

static int group_setup(void **state) {
	curl_global_init(CURL_GLOBAL_ALL);
	make_docker_context_default_local(&ctx);
	docker_context_result_handler_set(ctx, &handle_result_net);
	return E_SUCCESS;
}

static int group_teardown(void **state) {
	curl_global_cleanup();
	free(*state);
	free_docker_context(&ctx);
	return E_SUCCESS;
}

static void test_list_networks(void **state) {
	docker_network_list* networks;
	d_err_t e = docker_networks_list(ctx, &networks, NULL, NULL, NULL, NULL, NULL, NULL);
	assert_int_equal(e, E_SUCCESS);
	size_t len_nets = docker_network_list_length(networks);
	for(int i = 0; i < len_nets; i++) {
		docker_network* ni = docker_network_list_get_idx(networks, i);
		assert_non_null(ni);
		assert_non_null(docker_network_name_get(ni));
	}
	assert_int_equal(http_response_code, 200);
}

static void test_inspect_network(void **state) {
	docker_network* net;
	d_err_t e = docker_network_inspect(ctx, &net, "host", 0, NULL);
	assert_int_equal(e, E_SUCCESS);
	assert_int_equal(http_response_code, 200);
	assert_string_equal(docker_network_name_get(net), "host");
}

int docker_networks_tests() {
	const struct CMUnitTest tests[] = {
	cmocka_unit_test(test_list_networks),
	cmocka_unit_test(test_inspect_network)
 };
	return cmocka_run_group_tests_name("docker images tests", tests,
			group_setup, group_teardown);
}

