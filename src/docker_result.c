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

#include <stdlib.h>
#include <string.h>
#include "docker_result.h"

/**
 * Utility method to create docker result, should be used by all API
 * implementations to create the result object to return.
 *
 * Makes a defensive copy of all provided data so that they can be
 * freed after creation of the result.
 */
error_t make_docker_result(docker_result** result, error_t error_code,
		int http_error_code, const char* message) {
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
	return E_SUCCESS;
}

/**
 * Frees all internal memory used in the docker_result, should be
 * called for all result objects as soon as they are no longer needed.
 */
error_t free_docker_result(docker_result** result) {
	if ((*result)) {
		if ((*result)->message) {
			free((*result)->message);
		}
		free(*result);
	}
	return E_SUCCESS;
}

/**
 * This method provides the error_code based on the standard error code enum.
 * Use this method instead of direct attribute access to the struct to ensure
 * future changes to the struct will not break your code.
 */
error_t get_error(docker_result* result) {
	return result->error_code;
}

/**
 * This method provides the HTTP error code returned by the API call.
 * Use this method instead of direct attribute access to the struct to ensure
 * future changes to the struct will not break your code.
 */
int get_http_error(docker_result* result) {
	return result->http_error_code;
}

/**
 * This method provides the error message returned by the API.
 * Use this method instead of direct attribute access to the struct to ensure
 * future changes to the struct will not break your code.
 */
char* get_message(docker_result* result) {
	return result->message;
}
