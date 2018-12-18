/*
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
*/
/*
 * docker_connection_util.c
 *
 *  Created on: 11-Dec-2018
 *      Author: abhishek
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include "docker_connection_util.h"
#include "log.h"

docker_context* make_docker_context_url(const char* url) {
	docker_context* ctx = (docker_context*) malloc(sizeof(docker_context));
	char* u = (char*) malloc((strlen(url) + 1) * sizeof(char));
	strcpy(u, url);
	ctx->url = u;
	ctx->socket = NULL;
	return ctx;
}

docker_context* make_docker_context_socket(const char* socket) {
	docker_context* ctx = (docker_context*) malloc(sizeof(docker_context));
	char* s = (char*) malloc((strlen(socket) + 1) * sizeof(char));
	strcpy(s, socket);
	ctx->socket = s;
	ctx->url = NULL;
	return ctx;
}

/**
 * Free docker context memory.
 */
void free_docker_context(docker_context* ctx) {
	free((char *) ctx->url);
	free(ctx);
}

char* build_url(CURL *curl, char* base_url, url_param** params, int num_params) {
	if (params == NULL || num_params <= 0) {
		return base_url;
	} else {
		int size_toalloc = strlen(base_url) + 1;
		char** allkeys = (char**) malloc(num_params * sizeof(char*));
		char** allvals = (char**) malloc(num_params * sizeof(char*));
		for (int i = 0; i < num_params; i++) {
			log_debug("%s=%s\n", params[i]->k, params[i]->v);
			allkeys[i] = curl_easy_escape(curl, params[i]->k, 0);
			allvals[i] = curl_easy_escape(curl, params[i]->v, 0);
		}
		for (int i = 0; i < num_params; i++) {
			size_toalloc += strlen(allkeys[i]);
			size_toalloc += strlen(allvals[i]);
			size_toalloc += 2; //for ampersand and equals
		}
		char* url = (char*) malloc(sizeof(char) * size_toalloc);
		url[0] = '\0';
		strcat(url, base_url);
		if (num_params > 0) {
			strcat(url, "?");
			for (int i = 0; i < num_params; i++) {
				if (i > 0) {
					strcat(url, "&");
				}
				strcat(url, allkeys[i]);
				strcat(url, "=");
				strcat(url, allvals[i]);
			}
		}
		log_debug("URL Created:\n%s", url);
		return url;
	}
}

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb,
		void *userp) {
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *) userp;

	char *ptr = realloc(mem->memory, mem->size + realsize + 1);
	if (ptr == NULL) {
		/* out of memory! */
		log_debug("not enough memory (realloc returned NULL)");
		return 0;
	}

	mem->memory = ptr;
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

int set_curl_url(CURL* curl, docker_context* ctx, char* api_url,
		url_param** params, int num_params) {
	if (ctx->socket != NULL) {
		curl_easy_setopt(curl, CURLOPT_UNIX_SOCKET_PATH, ctx->socket);
		char* local_url = "http://localhost/";
		char* base_url = (char*)malloc(sizeof(char) * (strlen(local_url) + strlen(api_url) + 1));
		strcpy(base_url, local_url);
		strcat(base_url, api_url);
		char* url = build_url(curl, base_url, params, num_params);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		return 0;
	} else if (ctx->url != NULL) {
		char* base_url = (char*)malloc(sizeof(char) * (strlen(ctx->url) + strlen(api_url) + 1));
		strcpy(base_url, ctx->url);
		strcat(base_url, api_url);
		char* url = build_url(curl, base_url, params, num_params);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		return 0;
	} else {
		return -1;
	}
}

int docker_api_post(docker_context* ctx, char* api_url, url_param** params,
		int num_params, char* post_data, struct MemoryStruct *chunk) {
	CURL *curl;
	CURLcode res;
	struct curl_slist *headers = NULL;

	chunk->memory = malloc(1); /* will be grown as needed by the realloc above */
	chunk->size = 0; /* no data at this point */

	/* get a curl handle */
	curl = curl_easy_init();
	if (curl) {
		/* First set the URL that is about to receive our POST. This URL can
		 just as well be a https:// URL if that is what should receive the
		 data. */
		int set_url_err = set_curl_url(curl, ctx, api_url, params, num_params);
		if (set_url_err) {
			return -1;
		}
		headers = curl_slist_append(headers, "Expect:");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		/* Now specify the POST data */
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L);

		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void * )chunk);

		/* some servers don't like requests that are made without a user-agent
		 field, so we provide one */
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
		} else {
			/*
			 * Now, our chunk.memory points to a memory block that is chunk.size
			 * bytes big and contains the remote file.
			 *
			 * Do something nice with it!
			 */

			log_debug("%lu bytes retrieved\n", (unsigned long) chunk->size);
			//log_debug("Data is [%s].\n", chunk->memory);
		}
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	return 0;
}

int docker_api_get(docker_context* ctx, char* api_url, url_param** params,
		int num_params, struct MemoryStruct *chunk) {
	CURL *curl;
	CURLcode res;
	struct curl_slist *headers = NULL;

	chunk->memory = malloc(1); /* will be grown as needed by the realloc above */
	chunk->size = 0; /* no data at this point */

	/* get a curl handle */
	curl = curl_easy_init();
	if (curl) {
		/* First set the URL that is about to receive our POST. This URL can
		 just as well be a https:// URL if that is what should receive the
		 data. */
		int set_url_err = set_curl_url(curl, ctx, api_url, params, num_params);
		if (set_url_err) {
			return -1;
		}

		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void * )chunk);

		/* some servers don't like requests that are made without a user-agent
		 field, so we provide one */
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
		} else {
			/*
			 * Now, our chunk.memory points to a memory block that is chunk.size
			 * bytes big and contains the remote file.
			 *
			 * Do something nice with it!
			 */

			log_debug("%lu bytes retrieved\n", (unsigned long) chunk->size);
			//log_debug("Data is [%s].\n", chunk->memory);
		}
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	return 0;
}
