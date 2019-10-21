/*
* clibdocker: test_docker_networks.c
* Created on: 06-Jan-2019
* 
* clibdocker
* Copyright (C) 2018 Abhishek Mishra <abhishekmishra3@gmail.com>
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
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
static docker_result* res;

static int group_setup(void **state) {
	curl_global_init(CURL_GLOBAL_ALL);
	make_docker_context_default_local(&ctx);
	return E_SUCCESS;
}

static int group_teardown(void **state) {
	curl_global_cleanup();
	free(*state);
	free_docker_context(&ctx);
	return E_SUCCESS;
}

static void test_list_networks(void **state) {
	arraylist* networks;
	docker_networks_list(ctx, &res, &networks, NULL, NULL, NULL, NULL, NULL, NULL);
	handle_error(res);
	int len_nets = arraylist_length(networks);
	for(int i = 0; i < len_nets; i++) {
		docker_network* ni = (docker_network*)arraylist_get(networks, i);
		assert_non_null(ni->name);
	}
	assert_int_equal(res->http_error_code, 200);
}

static void test_inspect_network(void **state) {
	docker_network* net;
	docker_network_inspect(ctx, &res, &net, "host", 0, NULL);
	handle_error(res);
	assert_int_equal(res->http_error_code, 200);
	assert_string_equal(net->name, "host");
}

int docker_networks_tests() {
	const struct CMUnitTest tests[] = {
	cmocka_unit_test(test_list_networks),
	cmocka_unit_test(test_inspect_network)
 };
	return cmocka_run_group_tests_name("docker images tests", tests,
			group_setup, group_teardown);
}

