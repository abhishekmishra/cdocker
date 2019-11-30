/*
 * clibdocker: test_docker_system.c
 * Created on: 26-Dec-2018
 *
 * MIT License
 *
 * Copyright (c) 2018 Abhishek Mishra
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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

void handle_result_for_test(docker_result* result) {
	docker_log_info(result->url);
}

static int group_setup(void **state) {
	curl_global_init(CURL_GLOBAL_ALL);
	make_docker_context_default_local(&ctx);
	ctx->result_handler_fn = &handle_result_for_test;
	return E_SUCCESS;
}

static int group_teardown(void **state) {
	curl_global_cleanup();
	free(*state);
	free_docker_context(&ctx);
	return E_SUCCESS;
}

static void test_ping(void **state) {
	docker_ping(ctx, &res);
	handle_error(res);
	assert_int_equal(res->http_error_code, 200);
}

static void test_version(void **state) {
	docker_version* version;
	docker_system_version(ctx, &res, &version);
	handle_error(res);
	assert_int_equal(res->http_error_code, 200);
	assert_non_null(version);
	char* version_str = docker_version_version_get(version);
	assert_non_null(version_str);
	docker_log_info("Docker version is %s", version_str);
	free_docker_version(version);
}

static void test_info(void **state) {
	docker_info* info;
	docker_system_info(ctx, &res, &info);
	handle_error(res);
	assert_int_equal(res->http_error_code, 200);
	assert_non_null(info);
	unsigned long num_ctrs = docker_info_containers_get(info);
	docker_log_info("# of containers is %lu", num_ctrs);
	free_docker_version(info);
}

static void test_events(void **state) {
	docker_event_list* events;
	time_t now = time(NULL);
	docker_system_events(ctx, &res, &events, now - 360000, now);
	handle_error(res);
	assert_int_equal(res->http_error_code, 200);
	assert_non_null(events);
	assert_int_not_equal(docker_event_list_length(events), 0);
	for (size_t i = 0; i < docker_event_list_length(events); i++) {
		docker_event* evt = docker_event_list_get_idx(events, i);
		assert_non_null(docker_event_action_get(evt));
		docker_log_info("Event action is %s", docker_event_action_get(evt));
	}
}

static void test_df(void** state) {
	docker_df* df;
	docker_system_df(ctx, &res, &df);
	handle_error(res);
	assert_int_equal(res->http_error_code, 200);
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
