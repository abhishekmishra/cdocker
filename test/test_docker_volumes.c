/*
 * clibdocker: test_docker_volumes.c
 * Created on: 07-Jan-2019
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

#include "test_docker_volumes.h"

#include "docker_log.h"
#include "docker_volumes.h"
#include "docker_connection_util.h"
#include "test_util.h"

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

static void test_create_volumes(void **state) {
	docker_volume* vi = NULL;
	docker_volume_create(ctx, &res, &vi, "clibdocker_test_vol01", "local", 1,
			"clibdocker_test_label", "clibdocker_test_value");
	handle_error(res);
	assert_int_equal(res->http_error_code, 201);
	assert_non_null(vi);
	assert_non_null(vi->name);
	assert_string_equal(vi->name, "clibdocker_test_vol01");

	docker_volume_create(ctx, &res, &vi, "clibdocker_test_vol02", "local", 1,
			"clibdocker_test_label", "clibdocker_test_value");
	handle_error(res);
	assert_int_equal(res->http_error_code, 201);
	assert_non_null(vi);
	assert_non_null(vi->name);
	assert_string_equal(vi->name, "clibdocker_test_vol02");
}
static void test_inspect_volume(void **state) {
	docker_volume* vi = NULL;
	docker_volume_inspect(ctx, &res, &vi, "clibdocker_test_vol01");
	handle_error(res);
	assert_int_equal(res->http_error_code, 200);
	assert_non_null(vi);
	assert_non_null(vi->name);
	assert_string_equal(vi->name, "clibdocker_test_vol01");
}

static void test_list_volumes(void **state) {
	struct array_list* volumes;
	struct array_list* warnings;
	docker_volumes_list(ctx, &res, &volumes, &warnings, 1, NULL, "clibdocker_test_label=clibdocker_test_value", NULL);
	handle_error(res);
	int len_vols = array_list_length(volumes);
	assert_int_equal(res->http_error_code, 200);
	assert_int_equal(len_vols, 2);
}

static void test_delete_volume(void **state) {
	docker_volume_delete(ctx, &res, "clibdocker_test_vol01", 0);
	handle_error(res);
	assert_int_equal(res->http_error_code, 204);
}

static void test_prune_unused_volumes(void **state) {
	struct array_list* volumes_deleted;
	long space_reclaimed;
	docker_volumes_delete_unused(ctx, &res, &volumes_deleted, &space_reclaimed, 0, "clibdocker_test_label", "clibdocker_test_value");
	handle_error(res);
	for (int i = 0; i < array_list_length(volumes_deleted); i++) {
		docker_log_info("Deleted unused volume %s",
				(char* )array_list_get_idx(volumes_deleted, i));
	}
	assert_int_equal(res->http_error_code, 200);
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

