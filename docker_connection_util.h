/*
 * docker_connection_util.h
 *
 *  Created on: 11-Dec-2018
 *      Author: abhishek
 */

#ifndef DOCKER_CONNECTION_UTIL_H_
#define DOCKER_CONNECTION_UTIL_H_


#define URL "http://192.168.1.33:2376/"

struct MemoryStruct
{
    char *memory;
    size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

int docker_api_post(char* url, char* post_data, struct MemoryStruct *chunk);

int docker_api_get(char* url, struct MemoryStruct *chunk);

#endif /* DOCKER_CONNECTION_UTIL_H_ */
