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

/**
 * \file docker_connection_util.h
 * \brief Docker Connection Utils
 */

#ifndef DOCKER_CONNECTION_UTIL_H_
#define DOCKER_CONNECTION_UTIL_H_

#ifdef __cplusplus  
extern "C" {
#endif

#include <coll_arraylist.h>
#include <coll_arraylist_map.h>
#include <stdlib.h>
#include "docker_common.h"
#include <stdbool.h>
#include <json-c/json_object.h>
#include <json-c/json_tokener.h>
#include <json-c/linkhash.h>
#include <curl/curl.h>
#include "docker_result.h"

/**
 * @brief Currently supported Docker API Version
 */
#define DOCKER_API_VERSION_1_39 "1.39"

/**
 * @brief Docker UNIX Socket URL
 */
#define DOCKER_DEFAULT_UNIX_SOCKET "/var/run/docker.sock"

/**
 * @brief Window NPIPE URL Prefix
 */
#define DOCKER_NPIPE_URL_PREFIX "npipe://"

/**
 * @brief Default Windows Docker NPIPE URL
 */
#define DOCKER_DEFAULT_WINDOWS_NAMED_PIPE "npipe:////./pipe/docker_engine"

/**
 * @brief Default Docker HTTP URL
 */
#define DOCKER_DEFAULT_LOCALHOST_URL "http://localhost:2375/"

#if defined(_WIN32)
#define DOCKER_DEFAULT_SOCKET DOCKER_DEFAULT_WINDOWS_NAMED_PIPE
#endif

#if defined(unix) || defined(__unix__) || defined(__unix)
#define DOCKER_DEFAULT_SOCKET DOCKER_DEFAULT_UNIX_SOCKET
#endif

/**
 * @brief JSON Content Type Header
 */
#define HEADER_JSON "Content-Type: application/json"

/**
 * @brief TAR Content Type Header
 */
#define HEADER_TAR "Content-Type: application/x-tar"

/**
 * @brief Check if the given string input is a HTTP(s) URL.
 * 
 * @param url input string
 * @return bool whether string is an http url
 */
MODULE_API bool is_http_url(char* url);

/**
 * @brief Check if the given string input is a UNIX socket URL.
 * 
 * @param url input string
 * @return bool whether string is a Unix socket.
 */
MODULE_API bool is_unix_socket(char* url);

/**
 * @brief Check if the given string input is an NPIPE URL.
 * 
 * @param url input string
 * @return bool whether string is an Npipe url.
 */
MODULE_API bool is_npipe(char* url);

/**
 * @brief Get the docker api url part of the npipe url.
 * Removes the npipe:// prefix from the url.
 * 
 * @param url input string
 * @return char* a new string with the docker url part
 */
MODULE_API char* npipe_url_only(char* url);

/**
 * @brief Docker Object type in the Docker API call JSON
 */
typedef enum {
	NONE = 0, CONTAINER = 1, IMAGE = 2, SYSTEM = 3, NETWORK = 4, VOLUME = 5
} docker_object_type;

/**
 * @brief Defines a docker result handler function
 * 
 * This function type is used to define a result handler by a client
 * program. The result handler is called when the docker API call returns.
 */
typedef void (docker_result_handler_fn) (struct docker_context_t* ctx, docker_result* result);

/**
 * @brief A docker context for a specific docker server.
 */
typedef struct docker_context_t {
	char* url;										///< Url of the docker server
	char* api_version;								///< API version expected
	docker_result_handler_fn* result_handler_fn;	///< Result handler for all responses
	void* client_args;								///< Client args passed to callback functions
} docker_context;

/**
 * Global initialization of the docker API, should
 * be called only once in an application
 * 
 * @return error code 0 if initialization was ok, non-zero otherwise
 */
MODULE_API d_err_t docker_api_init();


/**
 * Global cleanup of the docker API,
 * should be called once when all api usage is complete.
 * 
*/
MODULE_API void docker_api_cleanup();

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

/**
 * @brief Get the docker context result handler function
 * 
 * @param ctx docker context
 * @return docker_result_handler_fn* result handler fn
 */
MODULE_API docker_result_handler_fn* docker_context_result_handler_get(docker_context* ctx);

/**
 * @brief Set the client args for the docker context
 * 
 * @param ctx docker context
 * @param client_args a client args value
 * @return d_err_t error code
 */
MODULE_API d_err_t docker_context_client_args_set(docker_context* ctx, void* client_args);

/**
 * @brief Get the client args for the docker context
 * 
 * @param ctx docker context
 * @return void* client args
 */
MODULE_API void* docker_context_client_args_get(docker_context* ctx);

/**
 * Free docker context memory.
 */
MODULE_API d_err_t free_docker_context(docker_context** ctx);

// BEGIN: Docker API Calls HTTP Utils V2 

/**
 * @brief Status callback function type. This is used to get a status callback from docker calls.
 */
typedef void (status_callback)(char* msg, void* cbargs, void* client_cbargs);

/**
 * @brief internal datastructure representing a Docker Call object.
 * 
 * All Docker API call implementations internally use this object
 * to represent a call to the Docker API.
 */
typedef struct docker_call_t {
	// URL Parts
	char* site_url;					///< site url
	docker_object_type object;		///< docker object type enum value
	char* id;						///< docker object id if applicable
	char* method;					///< docker api request method
	coll_al_map* params;			///< docker request parameters map

	// HTTP
	char* request_method;			///< http request method
	char* content_type_header;		///< http request content type header value

	// Request object
	char* request_data;				///< http request data
	size_t request_data_len;		///< http request data length

	// Response data
	// char* response_data;
	int http_error_code;			///< http response code

	// HTTP Response Internals
	char* memory;					///< internal memory used for response
	size_t capacity;				///< total capacity of internal response storage
	size_t size;					///< used size of the storage
	size_t flush_end;				///< size of storage already flushed

	// Callback Config
	status_callback* status_cb;		///< the status callback method
	void* cb_args;					///< callback args for internal usage
	void* client_cb_args;			///< callback args provided by client
} docker_call;

/**
 * @brief Create a new generic Docker API Call object.
 *
 * The Docker Call object has getters and setters of the form
 * \c docker_call_<member>_get , and
 * \c docker_call_<member>_set.
 * 
 * @param dcall pointer to \c docker_call* to create and return
 * @param site_url request url
 * @param object the docker object type whose endpoint is being called
 * @param id docker object id
 * @param method docker request http method
 * @return d_err_t error code
 */
MODULE_API d_err_t make_docker_call(docker_call** dcall, char* site_url, docker_object_type object, 
	const char* id,	const char* method);

/**
 * @brief Set the docker request HTTP method.
 * 
 * @param dcall docker call object
 * @param method http method
 */
MODULE_API void docker_call_request_method_set(docker_call* dcall, char* method);

/**
 * @brief Get the docker request HTTP method.
 * 
 * @param dcall docker call object
 * @return char* http method
 */
MODULE_API char* docker_call_request_method_get(docker_call* dcall);

/**
 * @brief Set the docker call content type header.
 * 
 * @param dcall docker call object
 * @param content_type_header content type header
 */
MODULE_API void docker_call_content_type_header_set(docker_call* dcall, char* content_type_header);

/**
 * @brief Get the docker request content type header.
 * 
 * @param dcall docker call object
 * @return char* content type header
 */
MODULE_API char* docker_call_content_type_header_get(docker_call* dcall);

/**
 * @brief Set the docker request data.
 * 
 * @param dcall docker call object
 * @param request_data json request data
 */
MODULE_API void docker_call_request_data_set(docker_call* dcall, char* request_data);

/**
 * @brief Get the docker request data.
 * 
 * @param dcall docker call object
 * @return char* request data
 */
MODULE_API char* docker_call_request_data_get(docker_call* dcall);

/**
 * @brief Set the docker request data length
 * 
 * @param dcall docker call object
 * @param request_data_len request data length
 */
MODULE_API void docker_call_request_data_len_set(docker_call* dcall, size_t request_data_len);

/**
 * @brief Get the docker request data length.
 * 
 * @param dcall docker call object
 * @return size_t request data length
 */
MODULE_API size_t docker_call_request_data_len_get(docker_call* dcall);

//void docker_call_response_data_set(docker_call* dcall, char* response_data);

/**
 * @brief Get the docker response data.
 * 
 * @param dcall docker call object
 * @return char* response data
 */
MODULE_API char* docker_call_response_data_get(docker_call* dcall);

/**
 * @brief Get the docker response data length.
 * 
 * @param dcall docker call object
 * @return size_t response data length
 */
MODULE_API size_t docker_call_response_data_length(docker_call* dcall);

/**
 * @brief Get the docker response HTTP code.
 * 
 * @param dcall docker call object
 * @return int HTTP response code
 */
MODULE_API int docker_call_http_code_get(docker_call* dcall);

/**
 * @brief Set the docker respose HTTP code.
 * 
 * @param dcall docker call object
 * @param http_code http response code
 */
MODULE_API void docker_call_http_code_set(docker_call* dcall, int http_code);

/**
 * @brief Set the docker call callback function.
 * 
 * @param dcall docker call object
 * @param status_callback* status callback function for the docker call
 */
MODULE_API void docker_call_status_cb_set(docker_call* dcall, status_callback* status_callback);

/**
 * @brief Get the docker call callback function
 * 
 * @param dcall docker call object
 * @return statuc_callback* callback function
 */
MODULE_API status_callback* docker_call_status_cb_get(docker_call* dcall);

/**
 * @brief Set the docker call callback function callback args.
 * 
 * @param dcall docker call object
 * @param cb_args callback args (argument to callback function)
 */
MODULE_API void docker_call_cb_args_set(docker_call* dcall, void* cb_args);

/**
 * @brief Get the docker call callback function callback args.
 * 
 * @param dcall docker call object
 * @return void* callback arguments
 */
MODULE_API void* docker_call_cb_args_get(docker_call* dcall);

/**
 * @brief Set the docker call client callback args
 * 
 * @param dcall docker call object
 * @param client_cb_args args set by the client of the api
 */
MODULE_API void docker_call_client_cb_args_set(docker_call* dcall, void* client_cb_args);

/**
 * @brief Get the docker call client callback args
 * 
 * @param dcall docker call object
 * @return void* client callback args
 */
MODULE_API void* docker_call_client_cb_args_get(docker_call* dcall);

/**
 * @brief Free the docker call object.
 * 
 * @param dcall docker call object
 */
MODULE_API void free_docker_call(docker_call* dcall);

/**
 * @brief Add a string key/value pair to the docker call parameters.
 * 
 * @param dcall docker call object
 * @param param parameter key
 * @param value parameter value
 * @return int error code
 */
MODULE_API int docker_call_params_add(docker_call* dcall, char* param, char* value);

/**
 * @brief Add a string key/boolean value pair to the docker call parameters.
 * 
 * @param dcall docker call object
 * @param param parameter key
 * @param value parameter value
 * @return int error code
 */
MODULE_API int docker_call_params_add_boolean(docker_call* dcall, char* param, int value);

/**
 * @brief Get the docker request HTTP url.
 * 
 * @param dcall docker call object
 * @return char* http url
 */
MODULE_API char* docker_call_get_url(docker_call* dcall);

/**
 * @brief Get the docker request service url.
 * 
 * @param dcall docker call object
 * @return char* service url
 */
MODULE_API char* docker_call_get_svc_url(docker_call* dcall);

/**
 * @brief Execute the Docker Call i.e. send the request to the server and get response.
 * 
 * @param ctx docker context
 * @param dcall docker call object
 * @param response json response object to be set
 * @return d_err_t error code
 */
MODULE_API d_err_t docker_call_exec(docker_context* ctx, docker_call* dcall, json_object** response);

// END: Docker API Calls HTTP Utils V2 

// BEGIN: Windows Named Pipe Support

#ifdef _WIN32
#include <windows.h> 
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

#define NPIPE_READ_BUFSIZE 1024

#endif

// END: Windows Named Pipe Support

#ifdef __cplusplus 
}
#endif

#endif /* DOCKER_CONNECTION_UTIL_H_ */
