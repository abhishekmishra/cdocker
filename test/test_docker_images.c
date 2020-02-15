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

#include "test_docker_images.h"

#include "docker_log.h"
#include "docker_images.h"
#include "docker_connection_util.h"
#include "test_util.h"

static docker_context* ctx = NULL;

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
	d_err_t e = docker_image_create_from_image(ctx, "alpine:latest", NULL, NULL);
	assert_int_equal(e, E_SUCCESS);
}

static void test_list_images(void **state) {
	docker_image_list* images;
	d_err_t e = docker_images_list(ctx, &images, 0, 1, NULL, 0, NULL, NULL, NULL);
	assert_int_equal(e, E_SUCCESS);
	assert_non_null(images);
	assert_int_not_equal(docker_image_list_length(images), 0);
	for (int i = 0; i < docker_image_list_length(images); i++) {
		docker_image* img = docker_image_list_get_idx(images, i);
		assert_non_null(docker_image_id_get(img));
		docker_log_info("Image found %s", docker_image_id_get(img));
	}
}

int docker_images_tests() {
	const struct CMUnitTest tests[] = {
	cmocka_unit_test(test_pull_alpine_latest),
	cmocka_unit_test(test_list_images) };
	return cmocka_run_group_tests_name("docker images tests", tests,
			group_setup, group_teardown);
}
