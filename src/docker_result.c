/*
 * clibdocker: docker_result.c
 * Created on: 18-Dec-2018
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "docker_result.h"
#include "log.h"

/**
 * Utility method to create docker result, should be used by all API
 * implementations to create the result object to return.
 *
 * Makes a defensive copy of all provided data so that they can be
 * freed after creation of the result.
 */
error_t make_docker_result(docker_result** result, error_t error_code,
		long http_error_code, const char* url, const char* message) {
	(*result) = (docker_result*) malloc(sizeof(docker_result));
	if ((*result) == NULL) {
		return E_ALLOC_FAILED;
	}
	(*result)->error_code = error_code;
	(*result)->http_error_code = http_error_code;
	if (message) {
		(*result)->message = (char*) malloc(
				sizeof(char) * (strlen(message) + 1));
		strcpy((*result)->message, message);
	} else {
		(*result)->message = NULL;
	}
	if (url) {
		(*result)->url = (char*) malloc(sizeof(char) * (strlen(url) + 1));
		strcpy((*result)->url, url);
	} else {
		(*result)->url = NULL;
	}
	(*result)->method = NULL;
	(*result)->request_json_str = NULL;
	(*result)->response_json_str = NULL;
	return E_SUCCESS;
}

/**
 * Frees all internal memory used in the docker_result, should be
 * called for all result objects as soon as they are no longer needed.
 */
void free_docker_result(docker_result** result) {
	if (result) {
		if ((*result)) {
			if ((*result)->message) {
				free((*result)->message);
			}
			if ((*result)->url) {
				free((*result)->url);
			}
			if ((*result)->request_json_str != NULL) {
				free((*result)->request_json_str);
			}
			if ((*result)->response_json_str != NULL) {
				free((*result)->response_json_str);
			}
			free(*result);
		}
	}
}

/**
 * This method provides the error_code based on the standard error code enum.
 * Use this method instead of direct attribute access to the struct to ensure
 * future changes to the struct will not break your code.
 */
error_t get_docker_result_error(docker_result* result) {
	return result->error_code;
}

/**
 * This method provides the HTTP error code returned by the API call.
 * Use this method instead of direct attribute access to the struct to ensure
 * future changes to the struct will not break your code.
 */
long get_docker_result_http_error(docker_result* result) {
	return result->http_error_code;
}

/**
 * This method provides the URL used when calling the API.
 * Use this method instead of direct attribute access to the struct to ensure
 * future changes to the struct will not break your code.
 */
char* get_docker_result_url(docker_result* result) {
	return result->url;
}

/**
 * This method provides the error message returned by the API.
 * Use this method instead of direct attribute access to the struct to ensure
 * future changes to the struct will not break your code.
 */
char* get_docker_result_message(docker_result* result) {
	return result->message;
}

/**
 * Check if the error_code is E_SUCCESS
 */
int is_ok(docker_result* result) {
	return (get_docker_result_error(result) == E_SUCCESS);
}

time_t get_docker_result_start_time(docker_result* result) {
	return result->start_time;
}

time_t get_docker_result_end_time(docker_result* result) {
	return result->end_time;
}

char* get_docker_result_request(docker_result* result) {
	return result->request_json_str;
}

char* get_docker_result_response(docker_result* result) {
	return result->response_json_str;
}

char* get_docker_result_http_method(docker_result* result) {
	return result->method;
}
/**
 * A simple error handler suitable for programs
 * which just want to log the error (if any).
 */
void docker_simple_error_handler_print(docker_result* res) {
	printf("DOCKER_RESULT: For URL: %s\n", get_docker_result_url(res));
	printf("DOCKER RESULT: Response error_code = %d, http_response = %ld\n",
			get_docker_result_error(res), get_docker_result_http_error(res));
	if (!is_ok(res)) {
		printf("DOCKER RESULT: %s\n", get_docker_result_message(res));
	}
	free_docker_result(&res);
}

/**
 * A simple error handler suitable for programs
 * which just want to log the error (if any).
 */
void docker_simple_error_handler_log(docker_result* res) {
	docker_log_debug("DOCKER_RESULT: For URL: %s", get_docker_result_url(res));
	docker_log_debug(
			"DOCKER RESULT: Response error_code = %d, http_response = %ld",
			get_docker_result_error(res), get_docker_result_http_error(res));
	if (!is_ok(res)) {
		docker_log_error("DOCKER RESULT: %s", get_docker_result_message(res));
	}
	free_docker_result(&res);
}
