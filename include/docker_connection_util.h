/*
 * docker_connection_util.h
 *
 *  Created on: 11-Dec-2018
 *      Author: abhishek
 */


/**
 * \file docker_connection_util.h
 * \brief Docker Connection Utils
 */

#ifndef DOCKER_CONNECTION_UTIL_H_
#define DOCKER_CONNECTION_UTIL_H_

#ifdef __cplusplus  
extern "C" {
#endif

#include <arraylist.h>
#include <coll_arraylist_map.h>
#include <stdlib.h>
#include "docker_common.h"
#include <stdbool.h>
#include <json-c/json_object.h>
#include <json-c/json_tokener.h>
#include <json-c/linkhash.h>
#include <curl/curl.h>
#include "docker_result.h"

#define DOCKER_API_VERSION_1_39 "1.39"
#define DOCKER_DEFAULT_UNIX_SOCKET "/var/run/docker.sock"
#define DOCKER_DEFAULT_WINDOWS_NAMED_PIPE "npipe://./pipe/docker_engine"
#define DOCKER_DEFAULT_LOCALHOST_URL "http://localhost:2375/"

#if defined(_WIN32)
#define DOCKER_DEFAULT_SOCKET DOCKER_DEFAULT_WINDOWS_NAMED_PIPE
#endif

#if defined(unix) || defined(__unix__) || defined(__unix)
#define DOCKER_DEFAULT_SOCKET DOCKER_DEFAULT_UNIX_SOCKET
#endif

#define HEADER_JSON "Content-Type: application/json"
#define HEADER_TAR "Content-Type: application/x-tar"

MODULE_API bool is_http_url(char* url);

MODULE_API bool is_unix_socket(char* url);

MODULE_API bool is_npipe(char* url);

typedef enum {
	NONE = 0, CONTAINER = 1, IMAGE = 2, SYSTEM = 3, NETWORK = 4, VOLUME = 5
} docker_object_type;

typedef void (docker_result_handler_fn) (struct docker_context_t* ctx, docker_result* result);

/**
 * A docker context for a specific docker server.
 */
typedef struct docker_context_t {
	char* url;
	char* api_version;
	docker_result_handler_fn* result_handler_fn;
	void* client_args;
} docker_context;

/**
 * Create a new docker context with the given url.
 *
 * The method makes a copy of the given string for use, so that it can
 * be safely freed by the calling program.
 */
MODULE_API d_err_t make_docker_context_url(docker_context** ctx, const char* url);

/**
 * Connect to the default local docker.
 *
 * @param ctx context to create
 * @return error code
 */
MODULE_API d_err_t make_docker_context_default_local(docker_context** ctx);

/**
 * Set the result handler function for the docker context.
 * Every API call's result will be passed to the result handler,
 * once the call is complete. 
 * NOTE: the result handler will be freed before the API call returns,
 * so if a copy of any of the data in the result is to be retained,
 * caller must make a copy.
 * 
 * @param ctx the docker context
 * @param result_handler_fn callback which receives the docker_result object.
 * @return error code
 */
MODULE_API d_err_t docker_context_result_handler_set(docker_context* ctx, docker_result_handler_fn* result_handler_fn);

MODULE_API docker_result_handler_fn* docker_context_result_handler_get(docker_context* ctx);

MODULE_API d_err_t docker_context_client_args_set(docker_context* ctx, void* client_args);

MODULE_API void* docker_context_client_args_get(docker_context* ctx);

/**
 * Free docker context memory.
 */
MODULE_API d_err_t free_docker_context(docker_context** ctx);

// BEGIN: Docker API Calls HTTP Utils V2 

typedef void (status_callback)(char* msg, void* cbargs, void* client_cbargs);

typedef struct docker_call_t {
	// URL Parts
	char* site_url;
	docker_object_type object;
	char* id;
	char* method;
	coll_al_map* params;

	// HTTP
	char* request_method;
	char* content_type_header;

	// Request object
	char* request_data;
	size_t request_data_len;

	// Response data
	// char* response_data;
	int http_error_code;

	// HTTP Response Internals
	char* memory;
	size_t capacity;
	size_t size;
	size_t flush_end;

	// Callback Config
	status_callback* status_cb;
	void* cb_args;
	void* client_cb_args;
} docker_call;

d_err_t make_docker_call(docker_call** dcall, char* site_url, docker_object_type object, 
	const char* id,	const char* method);

void docker_call_request_method_set(docker_call* dcall, char* method);

char* docker_call_request_method_get(docker_call* dcall);

void docker_call_content_type_header_set(docker_call* dcall, char* content_type_header);

char* docker_call_content_type_header_get(docker_call* dcall);

void docker_call_request_data_set(docker_call* dcall, char* request_data);

char* docker_call_request_data_get(docker_call* dcall);

void docker_call_request_data_len_set(docker_call* dcall, size_t request_data_len);

size_t docker_call_request_data_len_get(docker_call* dcall);

//void docker_call_response_data_set(docker_call* dcall, char* response_data);

char* docker_call_response_data_get(docker_call* dcall);

size_t docker_call_response_data_length(docker_call* dcall);

int docker_call_http_code_get(docker_call* dcall);

void docker_call_http_code_set(docker_call* dcall, int http_code);

void docker_call_status_cb_set(docker_call* dcall, status_callback* status_callback);

status_callback* docker_call_status_cb_get(docker_call* dcall);

void docker_call_cb_args_set(docker_call* dcall, void* cb_args);

char* docker_call_cb_args_get(docker_call* dcall);

void docker_call_client_cb_args_set(docker_call* dcall, void* client_cb_args);

char* docker_call_client_cb_args_get(docker_call* dcall);

void free_docker_call(docker_call* dcall);

int docker_call_params_add(docker_call* dcall, char* param, char* value);

char* docker_call_get_url(docker_call* dcall);

d_err_t docker_call_exec(docker_context* ctx, docker_call* dcall, json_object** response);

// END: Docker API Calls HTTP Utils V2 

#ifdef __cplusplus 
}
#endif

#endif /* DOCKER_CONNECTION_UTIL_H_ */
