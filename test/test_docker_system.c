/*
 *
 * Copyright (c) 2018-2022 Abhishek Mishra
 *
 * This file is part of clibdocker.
 *
 * clibdocker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation, 
 * either version 3 of the License, or (at your option) 
 * any later version.
 *
 * clibdocker is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty 
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public 
 * License along with clibdocker. 
 * If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <arraylist.h>

#include "test_docker_system.h"

#include "docker_util.h"
#include "docker_log.h"
#include "docker_system.h"
#include "docker_connection_util.h"
#include "test_util.h"

static docker_context* ctx = NULL;
static docker_result* res;

static int group_setup(void **state) {
	curl_global_init(CURL_GLOBAL_ALL);
	make_docker_context_default_local(&ctx);
	docker_context_result_handler_set(ctx, &handle_result_for_test);
	return E_SUCCESS;
}

static int group_teardown(void **state) {
	curl_global_cleanup();
	free(*state);
	free_docker_context(&ctx);
	return E_SUCCESS;
}

static void test_ping(void **state) {
	d_err_t e = docker_ping(ctx);
	assert_int_equal(e, E_SUCCESS);
}

static void test_version(void **state) {
	docker_version* version;
	d_err_t e = docker_system_version(ctx, &version);
	assert_int_equal(e, E_SUCCESS);
	assert_non_null(version);
	char* version_str = docker_version_version_get(version);
	assert_non_null(version_str);
	docker_log_info("Docker version is %s", version_str);
	free_docker_version(version);
}

static void test_info(void **state) {
	docker_info* info;
	d_err_t e = docker_system_info(ctx, &info);
	assert_int_equal(e, E_SUCCESS);
	assert_non_null(info);
	unsigned long num_ctrs = docker_info_containers_get(info);
	docker_log_info("# of containers is %lu", num_ctrs);
	free_docker_version(info);
}

static void test_events(void **state) {
	docker_event_list* events;
	time_t now = time(NULL);
	d_err_t e = docker_system_events(ctx, &events, now - 360000, now);
	assert_int_equal(e, E_SUCCESS);
	assert_non_null(events);
	assert_int_not_equal(docker_event_list_length(events), 0);
	for (size_t i = 0; i < docker_event_list_length(events); i++) {
		docker_event* evt = docker_event_list_get_idx(events, i);
		assert_non_null(docker_event_action_get(evt));
		docker_log_info("Event action is %s", docker_event_action_get(evt));
	}
	free_docker_event_list(events);
}

static void test_df(void** state) {
	docker_df* df;
	d_err_t e = docker_system_df(ctx, &df);
	assert_int_equal(e, E_SUCCESS);
	assert_non_null(df);
	unsigned long layers_size = docker_df_layers_size_get(df);
	docker_log_info("Layers Size is %lu", layers_size);
	free_docker_df(df);
}

int docker_system_tests() {
	const struct CMUnitTest tests[] = {
	cmocka_unit_test(test_ping),
	cmocka_unit_test(test_version),
	cmocka_unit_test(test_info),
	cmocka_unit_test(test_events),
	cmocka_unit_test(test_df),
 };
	return cmocka_run_group_tests_name("docker system tests", tests,
			group_setup, group_teardown);
}
