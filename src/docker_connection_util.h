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

#include "docker_common.h"
#include <stdbool.h>
#include <json-c/arraylist.h>
#include <json-c/json_object.h>
#include <json-c/json_tokener.h>
#include <json-c/linkhash.h>
#include <curl/curl.h>
#include "docker_result.h"

#define DOCKER_API_VERSION_1_39 "1.39"
#define DOCKER_DEFINE_DEFAULT_UNIX_SOCKET "/var/run/docker.sock"

MODULE_API bool is_http_url(char* url);

MODULE_API bool is_unix_socket(char* url);

typedef enum {
	CONTAINER = 1, IMAGE = 2, SYSTEM = 3, NETWORK = 4, VOLUME = 5
} docker_object_type;

/**
 * A docker context for a specific docker server.
 */
typedef struct docker_context {
	char* socket;
	char* url;
	char* api_version;
} docker_context;

/**
 * Create a new docker context with the given url.
 *
 * The method makes a copy of the given string for use, so that it can
 * be safely freed by the calling program.
 */
MODULE_API d_err_t make_docker_context_url(docker_context** ctx, const char* url);

/**
 * Create a new docker context with the given socket.
 *
 * The method makes a copy of the given string for use, so that it can
 * be safely freed by the calling program.
 */
MODULE_API d_err_t make_docker_context_socket(docker_context** ctx, const char* socket);

/**
 * Free docker context memory.
 */
MODULE_API d_err_t free_docker_context(docker_context** ctx);

/**
 * Url parameter structure which holds a string key (k) and string value (v).
 */
typedef struct url_param {
	char* k;
	char* v;
} url_param;

MODULE_API d_err_t make_url_param(url_param** p, char* key, char* value);
MODULE_API char* url_param_key(url_param* p);
MODULE_API char* url_param_value(url_param* p);
MODULE_API void free_url_param(url_param* p);

/**
 * Build a url given a CURL object, a base url and url parameters object (and it's length).
 */
MODULE_API char* build_url(CURL *curl, char* base_url, struct array_list* url_params);

MODULE_API d_err_t set_curl_url(CURL* curl, docker_context* ctx, char* api_url,
		struct array_list* url_params);

struct http_response_memory {
	char *memory;
	size_t size;
	int flush_end;
	void (*status_callback)(char* msg, void* cbargs, void* client_cbargs);
	void* cbargs;
	void* client_cbargs;
};

/**
 * Util method used internally to HTTP POST to the Docker url.
 */
MODULE_API d_err_t docker_api_post(docker_context* ctx, docker_result** res, char* api_url,
		struct array_list* url_params, const char* post_data,
		struct http_response_memory *chunk, json_object** response);

MODULE_API d_err_t docker_api_post_cb(docker_context* ctx, docker_result** result,
		char* api_url, struct array_list* url_params, const char* post_data,
		struct http_response_memory *chunk, json_object** response,
		void (*status_callback)(char* msg, void* cbargs, void* client_cbargs),
		void* cbargs, void* client_cbargs);

/**
 * Util method used internally to HTTP GET to the Docker url.
 */
MODULE_API d_err_t docker_api_get(docker_context* ctx, docker_result** res, char* api_url,
		struct array_list* url_params, struct http_response_memory *chunk,
		json_object** response);

MODULE_API d_err_t docker_api_get_cb(docker_context* ctx, docker_result** res,
		char* api_url, struct array_list* url_params,
		struct http_response_memory *chunk, json_object** response,
		void (*status_callback)(char* msg, void* cbargs, void* client_cbargs),
		void* cbargs, void* client_cbargs);

/**
 * Util method used internally to HTTP DELETE to the Docker url.
 */
MODULE_API d_err_t docker_api_delete(docker_context* ctx, docker_result** res, char* api_url,
		struct array_list* url_params, struct http_response_memory *chunk,
		json_object** response);

/**
 * Create service call part of the url using components.
 *
 * \param object which api to call (container, image etc. can also be null)
 * \param id if any to add to the url
 * \param method to call
 * \return string url
 */
MODULE_API char* create_service_url_id_method(docker_object_type object, const char* id,
		const char* method);

#ifdef __cplusplus 
}
#endif

#endif /* DOCKER_CONNECTION_UTIL_H_ */
