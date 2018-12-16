/*
 * docker_connection_util.h
 *
 *  Created on: 11-Dec-2018
 *      Author: abhishek
 */

#ifndef DOCKER_CONNECTION_UTIL_H_
#define DOCKER_CONNECTION_UTIL_H_

#include <curl/curl.h>

#define URL "http://192.168.1.33:2376/"

/**
* A docker context for a specific docker server.
*/
typedef struct docker_context {
	const char* url;
} docker_context;

/**
* Create a new docker context with the given url.
* The method makes a copy of the given string for use, so that it can
* be safely freed by the calling program.
*/
docker_context* make_docker_context(const char* url);

typedef struct url_param {
	char* k;
	char* v;
} url_param;

char* build_url(CURL *curl, char* base_url, url_param** params, int num_params);

struct MemoryStruct
{
    char *memory;
    size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

int docker_api_post(char* base_url, url_param** params, int num_params, char* post_data, struct MemoryStruct *chunk);

int docker_api_get(char* base_url, url_param** params, int num_params, struct MemoryStruct *chunk);

#endif /* DOCKER_CONNECTION_UTIL_H_ */
