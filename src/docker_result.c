/**
 * Copyright (c) 2020 Abhishek Mishra
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "docker_util.h"
#include <docker_log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "docker_result.h"

 /**
  * Utility method to create docker result, should be used by all API
  * implementations to create the result object to return.
  */
MODULE_API d_err_t new_docker_result(docker_result** result) {
	(*result) = (docker_result*)calloc(1, sizeof(docker_result));
	if ((*result) == NULL) {
		return E_ALLOC_FAILED;
	}
	(*result)->error_code = E_SUCCESS;
	(*result)->http_error_code = 0L;
	(*result)->start_time = 0;
	(*result)->end_time = 0;
	(*result)->url = NULL;
	(*result)->method = NULL;
	(*result)->request_json_str = NULL;
	(*result)->response_json_str = NULL;
	(*result)->message = NULL;
	return E_SUCCESS;
}

/**
 * Frees all internal memory used in the docker_result, should be
 * called for all result objects as soon as they are no longer needed.
 */
void free_docker_result(docker_result* result) {
	if (result) {
		if (result->message) {
			free(result->message);
		}
		if (result->url) {
			free(result->url);
		}
		if (result->request_json_str != NULL) {
			free(result->request_json_str);
		}
		if (result->response_json_str != NULL) {
			free(result->response_json_str);
		}
		free(result);
	}
}

docker_result* docker_result_clone(docker_result* result) {
	if (result != NULL) {
		docker_result* res_new;
		d_err_t e = new_docker_result(&res_new);
		if (e != E_SUCCESS) {
			return NULL;
		}
		res_new->error_code = result->error_code;
		res_new->http_error_code = result->http_error_code;
		res_new->start_time = result->start_time;
		res_new->end_time = result->end_time;
		res_new->url = str_clone(result->url);
		res_new->method = str_clone(result->method);
		res_new->request_json_str = str_clone(result->request_json_str);
		res_new->response_json_str = str_clone(result->response_json_str);
		res_new->message = str_clone(result->message);
		return res_new;
	}
	return NULL;
}

d_err_t docker_result_get_error_code(docker_result* result) {
	if (result != NULL) {
		return result->error_code;
	}
	else {
		return E_UNKNOWN_ERROR;
	}
}

time_t docker_result_get_start_time(docker_result* result) {
	if (result != NULL) {
		return result->start_time;
	}
	else {
		return 0;
	}
}

time_t docker_result_get_end_time(docker_result* result) {
	if (result != NULL) {
		return result->end_time;
	}
	else {
		return 0;
	}
}

char* docker_result_get_url(docker_result* result) {
	if (result != NULL) {
		return result->url;
	}
	else {
		return NULL;
	}
}

char* docker_result_get_method(docker_result* result) {
	if (result != NULL) {
		return result->method;
	}
	else {
		return NULL;
	}
}

char* docker_result_get_request_json_str(docker_result* result) {
	if (result != NULL) {
		return result->request_json_str;
	}
	else {
		return NULL;
	}
}

char* docker_result_get_response_json_str(docker_result* result) {
	if (result != NULL) {
		return result->response_json_str;
	}
	else {
		return NULL;
	}
}

long docker_result_get_http_error_code(docker_result* result) {
	if (result != NULL) {
		return result->http_error_code;
	}
	else {
		return 0L;
	}
}

char* docker_result_get_message(docker_result* result) {
	if (result != NULL) {
		return result->message;
	}
	else {
		return NULL;
	}
}

/**
 * Check if the error_code is E_SUCCESS
 */
int is_ok(docker_result* result) {
	return (result->error_code == E_SUCCESS);
}

/**
 * A simple error handler suitable for programs
 * which just want to log the error (if any).
 */
void docker_simple_error_handler_print(docker_result* res) {
	printf("DOCKER_RESULT: For URL: %s\n", res->url);
	printf("DOCKER RESULT: Response error_code = %d, http_response = %ld\n",
		res->error_code, res->http_error_code);
	if (!is_ok(res)) {
		printf("DOCKER RESULT: %s\n", res->message);
	}
}

/**
 * A simple error handler suitable for programs
 * which just want to log the error (if any).
 */
void docker_simple_error_handler_log(docker_result* res) {
	docker_log_debug("DOCKER_RESULT: For URL: %s", res->url);
	docker_log_debug(
		"DOCKER RESULT: Response error_code = %d, http_response = %ld",
		res->error_code, res->http_error_code);
	if (!is_ok(res)) {
		docker_log_error("DOCKER RESULT: %s", res->message);
	}
}
