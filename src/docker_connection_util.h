/*
 * docker_connection_util.h
 *
 *  Created on: 11-Dec-2018
 *      Author: abhishek
 */

#ifndef DOCKER_CONNECTION_UTIL_H_
#define DOCKER_CONNECTION_UTIL_H_

#include <curl/curl.h>
#include "docker_result.h"

/**
 * A docker context for a specific docker server.
 */
typedef struct docker_context {
	char* socket;
	char* url;
} docker_context;

/**
 * Create a new docker context with the given url.
 *
 * The method makes a copy of the given string for use, so that it can
 * be safely freed by the calling program.
 */
error_t make_docker_context_url(docker_context** ctx, const char* url);

/**
 * Create a new docker context with the given socket.
 *
 * The method makes a copy of the given string for use, so that it can
 * be safely freed by the calling program.
 */
error_t make_docker_context_socket(docker_context** ctx, const char* socket);

/**
 * Free docker context memory.
 */
error_t free_docker_context(docker_context** ctx);

/**
 * Url parameter structure which holds a string key (k) and string value (v).
 */
typedef struct url_param {
	char* k;
	char* v;
} url_param;

/**
 * Build a url given a CURL object, a base url and url parameters object (and it's length).
 */
char* build_url(CURL *curl, char* base_url, url_param** params, int num_params);

struct MemoryStruct {
	char *memory;
	size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb,
		void *userp);

/**
 * Util method used internally to HTTP POST to the Docker url.
 */
error_t docker_api_post(docker_context* ctx, char* api_url, url_param** params,
		int num_params, char* post_data, struct MemoryStruct *chunk);

/**
 * Util method used internally to HTTP GET to the Docker url.
 */
error_t docker_api_get(docker_context* ctx, char* api_url, url_param** params,
		int num_params, struct MemoryStruct *chunk);

#endif /* DOCKER_CONNECTION_UTIL_H_ */
