/*
 * clibdocker: docker_result.h
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

#ifndef SRC_DOCKER_RESULT_H_
#define SRC_DOCKER_RESULT_H_

#define DOCKER_PARAM_TRUE 1
#define DOCKER_PARAM_FALSE 0

// The error code usage below based on suggestions at
// https://stackoverflow.com/questions/6286874/c-naming-suggestion-for-error-code-enums

enum _config_error {
	E_SUCCESS = 0,
	E_INVALID_INPUT = -1,
	E_FILE_NOT_FOUND = -2,
	E_ALLOC_FAILED = -3,
	E_PING_FAILED = -4
};

/* type to provide in your API */
typedef enum _config_error error_t;

/* use this to provide a perror style method to help consumers out */
//struct _errordesc {
//	int code;
//	char *message;
//} errordesc[] = { { E_SUCCESS, "No error" },
//		{ E_INVALID_INPUT, "Invalid input" }, { E_FILE_NOT_FOUND,
//				"File not found" }, };
/**
 * The universal result object for docker API calls in this library.
 * This struct and associated methods provide ways to extract
 * success/failure state of the API Call, and messages associated with
 * any error.
 *
 * The struct must be inspected before the result values of the API
 * calls are used to ensure safe access.
 *
 * Also the result object must be freed using free_docker_result,
 * as soon as it is no longer needed.
 */
typedef struct docker_result_t {
	error_t error_code;
	long http_error_code;
	char* url;
	char* message;
} docker_result;

/**
 * Utility method to create docker result, should be used by all API
 * implementations to create the result object to return.
 *
 * Makes a defensive copy of all provided data so that they can be
 * freed after creation of the result.
 */
error_t make_docker_result(docker_result** result, error_t error_code,
		long http_error_code, const char* url, const char* msg);

/**
 * Frees all internal memory used in the docker_result, should be
 * called for all result objects as soon as they are no longer needed.
 */
error_t free_docker_result(docker_result** result);

/**
 * This method provides the error_code based on the standard error code enum.
 * Use this method instead of direct attribute access to the struct to ensure
 * future changes to the struct will not break your code.
 */
error_t get_error(docker_result* result);

/**
 * This method provides the HTTP error code returned by the API call.
 * Use this method instead of direct attribute access to the struct to ensure
 * future changes to the struct will not break your code.
 */
long get_http_error(docker_result* result);

/**
 * This method provides the URL used when calling the API.
 * Use this method instead of direct attribute access to the struct to ensure
 * future changes to the struct will not break your code.
 */
char* get_url(docker_result* result);

/**
 * This method provides the error message returned by the API.
 * Use this method instead of direct attribute access to the struct to ensure
 * future changes to the struct will not break your code.
 */
char* get_message(docker_result* result);

/**
 * Check if the error_code is E_SUCCESS
 */
int is_ok(docker_result* result);


//TODO these two functions can be made a single generic function.
/**
 * A simple error handler suitable for programs
 * which just want to log the error (if any).
 */
void docker_simple_error_handler_print(docker_result* res);

/**
 * A simple error handler suitable for programs
 * which just want to log the error (if any).
 */
void docker_simple_error_handler_log(docker_result* res);

#endif /* SRC_DOCKER_RESULT_H_ */
