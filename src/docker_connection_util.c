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

docker_context* make_docker_context(const char* url) {
	docker_context* ctx = (docker_context*)malloc(sizeof(docker_context));
	char* u = (char*)malloc((strlen(url) + 1) * sizeof(char));
	strcpy(u, url);
	ctx->url = u;
	return ctx;
}

char* build_url(CURL *curl, char* base_url, url_param** params, int num_params) {
	if (params == NULL || num_params <= 0) {
		return base_url;
	} else {
		int size_toalloc = strlen(base_url) + 1;
		char** allkeys = (char**) malloc(num_params * sizeof(char*));
		char** allvals = (char**) malloc(num_params * sizeof(char*));
		for (int i = 0; i < num_params; i++) {
			printf("%s=%s\n", params[i]->k, params[i]->v);
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
		printf("URL Created:\n%s\n", url);
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
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	mem->memory = ptr;
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

int docker_api_post(char* base_url, url_param** params, int num_params,
		char* post_data, struct MemoryStruct *chunk) {
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
		char* url = build_url(curl, base_url, params, num_params);
		curl_easy_setopt(curl, CURLOPT_URL, url);

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

			printf("%lu bytes retrieved\n", (unsigned long) chunk->size);
			//printf("Data is [%s].\n", chunk->memory);
		}
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	return 0;
}

int docker_api_get(char* base_url, url_param** params, int num_params,
		struct MemoryStruct *chunk) {
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
		char* url = build_url(curl, base_url, params, num_params);
		curl_easy_setopt(curl, CURLOPT_URL, url);

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

			printf("%lu bytes retrieved\n", (unsigned long) chunk->size);
			//printf("Data is [%s].\n", chunk->memory);
		}
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	return 0;
}
