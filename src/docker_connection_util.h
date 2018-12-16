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
