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
static int http_response_code = 0;

void handle_result(docker_context* ctx, docker_result* res) {
	http_response_code = docker_result_get_http_error_code(res);
}

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
	docker_context_result_handler_set(ctx, &handle_result);
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

void test_log_line_handler(void* args, int stream_id, int line_num, char* line) {
	if (line_num == 0) {
		assert_string_equal(line, "hello world\n");
		docker_log_info("Stream %d, line# %d :: %s", stream_id, line_num, line);
	}
}

static void test_start(void **state) {
	char* id = *state;
	d_err_t e = docker_start_container(ctx, id, NULL);
	assert_int_equal(e, E_SUCCESS);
	e = docker_wait_container(ctx, id, NULL);
	assert_int_equal(e, E_SUCCESS);
	char* output;
	size_t output_len;
	e = docker_container_logs(ctx, &output, &output_len, id, DOCKER_PARAM_FALSE,
	DOCKER_PARAM_TRUE, DOCKER_PARAM_FALSE, -1, -1, DOCKER_PARAM_FALSE, -1);
	assert_int_equal(e, E_SUCCESS);
	assert_non_null(output);
	docker_container_logs_foreach(NULL, output, output_len, &test_log_line_handler);
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
	d_err_t e = docker_container_list(ctx, &containers, 0, 5, 1, "id", id, NULL);
	assert_int_equal(e, E_SUCCESS);
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
	d_err_t e = docker_container_changes(ctx, &changes, id);
	assert_int_equal(e, E_SUCCESS);
	assert_null(changes);
}

static void test_stopping_stopped_container(void **state) {
	char* id = *state;
	d_err_t e = docker_stop_container(ctx, id, 0);
	assert_int_equal(e, E_INVALID_INPUT);
	assert_int_equal(http_response_code, 304L);
	//free_docker_result(res);
}

static void test_killing_stopped_container(void **state) {
	char* id = *state;
	d_err_t e = docker_kill_container(ctx, id, NULL);
	assert_int_equal(e, E_INVALID_INPUT);
	assert_int_equal(http_response_code, 409);
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
	d_err_t e = docker_pause_container(ctx, id);
	assert_int_equal(e, E_INVALID_INPUT);
	assert_int_equal(http_response_code, 409);
}

static void test_unpause_stopped_container(void **state) {
	char* id = *state;
	d_err_t e = docker_unpause_container(ctx, id);
	assert_int_equal(e, E_INVALID_INPUT);
	assert_int_equal(http_response_code, 500);
}

static void test_restart_container(void **state) {
	char* id = *state;
	d_err_t e = docker_restart_container(ctx, id, 0);
	assert_int_equal(e, E_SUCCESS);
	e = docker_wait_container(ctx, id, NULL);
	assert_int_equal(e, E_SUCCESS);
}

static void test_stats_container(void **state) {
	char* id = NULL;
	d_err_t e = docker_image_create_from_image_cb(ctx, &log_pull_message, NULL,
			"bfirsh/reticulate-splines", "latest", NULL);
	assert_int_equal(e, E_SUCCESS);

	docker_ctr_create_params* p = make_docker_ctr_create_params();
	docker_ctr_create_params_image_set(p, "bfirsh/reticulate-splines");
	e = docker_create_container(ctx, &id, p);
	assert_int_equal(e, E_SUCCESS);
	free_docker_ctr_create_params(p);

	docker_log_info("Started docker container id is %s\n", id);

	e = docker_start_container(ctx, id, NULL);
	assert_int_equal(e, E_SUCCESS);

	docker_container_stats* stats;
	e = docker_container_get_stats(ctx, &stats, id);
	assert_int_equal(e, E_SUCCESS);
	docker_container_cpu_stats* cpu_stats =
		docker_container_stats_cpu_stats_get(stats);
	docker_log_info("Cpu usage is %lu, num cpus is %d, usage%% is %f",
		docker_container_cpu_stats_system_cpu_usage_get(cpu_stats),
		docker_container_cpu_stats_online_cpus_get(cpu_stats),
		docker_container_stats_get_cpu_usage_percent(cpu_stats));

	e = docker_stop_container(ctx, id, 0);
	assert_int_equal(e, E_SUCCESS);
}

int docker_container_tests() {
	const struct CMUnitTest tests[] = {
	//	cmocka_unit_test(test_rename_stopped_container),
			cmocka_unit_test(test_start),
			//cmocka_unit_test(test_inspect),
			//cmocka_unit_test(test_list),
			//cmocka_unit_test(test_changes),
			//cmocka_unit_test(test_stopping_stopped_container),
			//cmocka_unit_test(test_killing_stopped_container),
			//cmocka_unit_test(test_pause_stopped_container),
			//cmocka_unit_test(test_unpause_stopped_container),
			//cmocka_unit_test(test_restart_container),
			//cmocka_unit_test(test_stats_container)
		};
	return cmocka_run_group_tests_name("docker container tests", tests,
			group_setup, group_teardown);
}
