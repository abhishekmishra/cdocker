/*
 * clibdocker: test_docker_images.c
 * Created on: 05-Jan-2019
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

#include "test_docker_images.h"

#include "docker_log.h"
#include "docker_images.h"
#include "docker_connection_util.h"
#include "test_util.h"

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

static void test_pull_alpine_latest(void **state) {
	docker_image_create_from_image(ctx, &res, "alpine:latest", NULL, NULL);
	handle_error(res);
	assert_int_equal(res->http_error_code, 200);
}

static void test_list_images(void **state) {
	docker_image_list* images;
	docker_images_list(ctx, &res, &images, 0, 1, NULL, 0, NULL, NULL, NULL);
	handle_error(res);
	assert_int_equal(res->http_error_code, 200);
	assert_non_null(images);
	assert_int_not_equal(docker_image_list_length(images), 0);
}

int docker_images_tests() {
	const struct CMUnitTest tests[] = {
	cmocka_unit_test(test_pull_alpine_latest),
	cmocka_unit_test(test_list_images) };
	return cmocka_run_group_tests_name("docker images tests", tests,
			group_setup, group_teardown);
}
