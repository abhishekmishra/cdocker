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

#include "docker_log.h"
#include "docker_containers.h"
#include "docker_images.h"
#include "docker_connection_util.h"
#include "test_util.h"

static docker_context* ctx = NULL;
static docker_result* res;

void log_pull_message(docker_image_create_status* status, void* client_cbargs) {
	if (status) {
		if (status->id) {
			docker_log_debug("message is %s, id is %s", status->status,
					status->id);
		} else {
			docker_log_debug("message is %s", status->status);
		}
	}
}

void log_stats(docker_container_stats* stats, void* client_cbargs) {
	if (stats) {
		docker_container_cpu_stats* cpu_stats =
				docker_container_stats_cpu_stats_get(stats);
		docker_log_info("Cpu usage is %lu, num cpus is %d, usage%% is %f",
				docker_container_cpu_stats_system_cpu_usage_get(cpu_stats), 
				docker_container_cpu_stats_online_cpus_get(cpu_stats),
				docker_container_stats_get_cpu_usage_percent(cpu_stats));
	}
}

static int group_setup(void **state) {
	char* id = NULL;
	docker_ctr_create_params* p;

	curl_global_init(CURL_GLOBAL_ALL);
	make_docker_context_default_local(&ctx);
	p = make_docker_ctr_create_params();
	docker_ctr_create_params_image_set(p, "alpine");
	docker_ctr_create_params_cmd_add(p, "echo");
	docker_ctr_create_params_cmd_add(p, "hello world");
	d_err_t e = docker_create_container(ctx, &id, p);
	assert_int_equal(e, E_SUCCESS);
	assert_non_null(id);
	*state = id;
	printf("id in state = %s\n", id);
	free_docker_ctr_create_params(p);
	return 0;
}
static int group_teardown(void **state) {
	curl_global_cleanup();
	free(*state);
	free_docker_context(&ctx);
	return 0;
}

static void test_start(void **state) {
	char* id = *state;
	docker_start_container(ctx, &res, id, NULL);
	handle_error(res);
	docker_wait_container(ctx, &res, id, NULL);
	handle_error(res);
	char* output;
	docker_container_logs(ctx, &res, &output, id, DOCKER_PARAM_FALSE,
	DOCKER_PARAM_TRUE, DOCKER_PARAM_FALSE, -1, -1, DOCKER_PARAM_FALSE, -1);
	handle_error(res);
	assert_non_null(output);
	assert_string_equal(output, "hello world\n");
}

static void test_inspect(void** state) {
	char* id = *state;
	docker_ctr* ctr = docker_inspect_container(ctx, id, 0);
	assert_non_null(ctr);
	//docker_log_info("%s", get_json_string(ctr));
	docker_log_info("%s", docker_ctr_name_get(ctr));
}

static void test_list(void **state) {
	char* id = *state;
	docker_ctr_list* containers = NULL;
	docker_container_list(ctx, &containers, 0, 5, 1, "id", id, NULL);
	docker_log_info("Read %d containers.\n",
			docker_ctr_list_length(containers));
	assert_non_null(containers);
	assert_int_equal(docker_ctr_list_length(containers), 1);
}

// TODO: need a better test case, when we have a long
// running instance which can be killed post the test.
static void test_changes(void **state) {
	char* id = *state;
	docker_changes_list* changes;
	//changes are empty because the instance is stopped by this time.
	docker_container_changes(ctx, &res, &changes, id);
	handle_error(res);
	assert_null(changes);
}

static void test_stopping_stopped_container(void **state) {
	char* id = *state;
	docker_stop_container(ctx, &res, id, 0);
	handle_error(res);
	assert_int_equal(res->http_error_code, 304L);
	//free_docker_result(res);
}

static void test_killing_stopped_container(void **state) {
	char* id = *state;
	docker_kill_container(ctx, &res, id, NULL);
	handle_error(res);
	assert_int_equal(res->http_error_code, 409);
}

//TODO: will need to create test for rename
//which renames and then restores name.
//static void test_rename_stopped_container(void **state) {
//	char* id = *state;
//	docker_rename_container(ctx, &res, id, "somename");
//	handle_error(res);
//	assert_int_equal(res->http_error_code, 204);
//}

static void test_pause_stopped_container(void **state) {
	char* id = *state;
	docker_pause_container(ctx, &res, id);
	handle_error(res);
	assert_int_equal(res->http_error_code, 409);
}

static void test_unpause_stopped_container(void **state) {
	char* id = *state;
	docker_unpause_container(ctx, &res, id);
	handle_error(res);
	assert_int_equal(res->http_error_code, 500);
}

static void test_restart_container(void **state) {
	char* id = *state;
	docker_restart_container(ctx, &res, id, 0);
	handle_error(res);
	assert_int_equal(res->http_error_code, 204);
	docker_wait_container(ctx, &res, id, NULL);
	handle_error(res);
	assert_int_equal(res->http_error_code, 200);
}

static void test_stats_container(void **state) {
	char* id = NULL;
	docker_image_create_from_image_cb(ctx, &res, &log_pull_message, NULL,
			"bfirsh/reticulate-splines", "latest", NULL);
	handle_error(res);

	docker_ctr_create_params* p = make_docker_ctr_create_params();
	docker_ctr_create_params_image_set(p, "bfirsh/reticulate-splines");
	d_err_t e = docker_create_container(ctx, &id, p);
	assert_int_equal(e, E_SUCCESS);
	free_docker_ctr_create_params(p);

	docker_log_info("Started docker container id is %s\n", id);

	docker_start_container(ctx, &res, id, NULL);
	handle_error(res);

	docker_container_stats* stats;
	docker_container_get_stats(ctx, &res, &stats, id);
	handle_error(res);

	docker_container_cpu_stats* cpu_stats =
		docker_container_stats_cpu_stats_get(stats);
	docker_log_info("Cpu usage is %lu, num cpus is %d, usage%% is %f",
		docker_container_cpu_stats_system_cpu_usage_get(cpu_stats),
		docker_container_cpu_stats_online_cpus_get(cpu_stats),
		docker_container_stats_get_cpu_usage_percent(cpu_stats));

	assert_int_equal(res->http_error_code, 200);

	docker_stop_container(ctx, &res, id, 0);
	handle_error(res);
}

int docker_container_tests() {
	const struct CMUnitTest tests[] = {
	//	cmocka_unit_test(test_rename_stopped_container),
			cmocka_unit_test(test_start),
			cmocka_unit_test(test_inspect),
			cmocka_unit_test(test_list),
			cmocka_unit_test(test_changes),
			cmocka_unit_test(test_stopping_stopped_container),
			cmocka_unit_test(test_killing_stopped_container),
			cmocka_unit_test(test_pause_stopped_container),
			cmocka_unit_test(test_unpause_stopped_container),
			cmocka_unit_test(test_restart_container),
			cmocka_unit_test(test_stats_container)
		};
	return cmocka_run_group_tests_name("docker container tests", tests,
			group_setup, group_teardown);
}
