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

#include "test_docker_volumes.h"

#include "docker_log.h"
#include "docker_volumes.h"
#include "docker_connection_util.h"
#include "test_util.h"

static docker_context* ctx = NULL;
static int http_response_code = 0;

void handle_result_vol(docker_context* ctx, docker_result* res) {
	http_response_code = docker_result_get_http_error_code(res);
}

static int group_setup(void **state) {
	curl_global_init(CURL_GLOBAL_ALL);
	make_docker_context_default_local(&ctx);
	docker_context_result_handler_set(ctx, &handle_result_vol);
	return E_SUCCESS;
}

static int group_teardown(void **state) {
	curl_global_cleanup();
	free(*state);
	free_docker_context(&ctx);
	return E_SUCCESS;
}

static void test_create_volumes(void **state) {
	docker_volume* vi = NULL;
	d_err_t e = docker_volume_create(ctx, &vi, "clibdocker_test_vol01", "local", 1,
			"clibdocker_test_label", "clibdocker_test_value");
	assert_int_equal(e, E_SUCCESS);
	assert_int_equal(http_response_code, 201);
	assert_non_null(vi);
	assert_non_null(docker_volume_name_get(vi));
	assert_string_equal(docker_volume_name_get(vi), "clibdocker_test_vol01");

	e = docker_volume_create(ctx, &vi, "clibdocker_test_vol02", "local", 1,
			"clibdocker_test_label", "clibdocker_test_value");
	assert_int_equal(e, E_SUCCESS);
	assert_int_equal(http_response_code, 201);
	assert_non_null(vi);
	assert_non_null(docker_volume_name_get(vi));
	assert_string_equal(docker_volume_name_get(vi), "clibdocker_test_vol02");
}
static void test_inspect_volume(void **state) {
	docker_volume* vi = NULL;
	d_err_t e = docker_volume_inspect(ctx, &vi, "clibdocker_test_vol01");
	assert_int_equal(e, E_SUCCESS);
	assert_int_equal(http_response_code, 200);
	assert_non_null(vi);
	assert_non_null(docker_volume_name_get(vi));
	assert_string_equal(docker_volume_name_get(vi), "clibdocker_test_vol01");
}

static void test_list_volumes(void **state) {
	docker_volume_list* volumes;
	docker_volume_warnings* warnings;
	d_err_t e = docker_volumes_list(ctx, &volumes, &warnings, 1, NULL, "clibdocker_test_label=clibdocker_test_value", NULL);
	assert_int_equal(e, E_SUCCESS);
	size_t len_vols = docker_volume_list_length(volumes);
	assert_int_equal(http_response_code, 200);
	assert_int_equal(len_vols, 2);
}

static void test_delete_volume(void **state) {
	d_err_t e = docker_volume_delete(ctx, "clibdocker_test_vol01", 0);
	assert_int_equal(e, E_SUCCESS);
	assert_int_equal(http_response_code, 204);
}

static void test_prune_unused_volumes(void **state) {
	arraylist* volumes_deleted;
	unsigned long space_reclaimed;
	d_err_t e = docker_volumes_delete_unused(ctx, &volumes_deleted, &space_reclaimed, 0, "clibdocker_test_label", "clibdocker_test_value");
	assert_int_equal(e, E_SUCCESS);
	for (int i = 0; i < arraylist_length(volumes_deleted); i++) {
		docker_log_info("Deleted unused volume %s",
				(char* )arraylist_get(volumes_deleted, i));
	}
	assert_int_equal(http_response_code, 200);
}

int docker_volumes_tests() {
	const struct CMUnitTest tests[] = {
	cmocka_unit_test(test_create_volumes),
	cmocka_unit_test(test_inspect_volume),
	cmocka_unit_test(test_list_volumes),
	cmocka_unit_test(test_delete_volume),
	cmocka_unit_test(test_prune_unused_volumes)};
	return cmocka_run_group_tests_name("docker volumes tests", tests,
			group_setup, group_teardown);
}

