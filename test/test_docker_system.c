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

#include "test_docker_system.h"
#include "docker_system.h"
#include "docker_connection_util.h"
#include "test_util.h"
#include "log.h"

static docker_context* ctx = NULL;
static docker_result* res;

static int group_setup(void **state) {
	curl_global_init(CURL_GLOBAL_ALL);
	make_docker_context_socket(&ctx, "/var/run/docker.sock");
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
	assert_non_null(version->version);
}


int docker_system_tests() {
	const struct CMUnitTest tests[] = {
	cmocka_unit_test(test_ping),
	cmocka_unit_test(test_version)
 };
	return cmocka_run_group_tests_name("docker system tests", tests,
			group_setup, group_teardown);
}
