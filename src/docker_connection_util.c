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
#include "docker_util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <docker_log.h>
#include "docker_result.h"
#include "docker_connection_util.h"
#include <json-c/json_object.h>
#include <json-c/json_tokener.h>
#include <json-c/linkhash.h>
#include <stdbool.h>

bool prefix(const char* pre, const char* str)
{
	return strncmp(pre, str, strlen(pre)) == 0;
}

bool is_http_url(char* url)
{
	if (url)
	{
		if (strlen(url) > 0)
		{
			return prefix("http", url);
		}
	}
	return false;
}

bool is_unix_socket(char* url)
{
	if (url)
	{
		if (strlen(url) > 0)
		{
			return prefix("/", url);
		}
	}
	return false;
}

bool is_npipe(char* url)
{
	if (url)
	{
		if (strlen(url) > 0)
		{
			return prefix("npipe", url);
		}
	}
	return false;
}


d_err_t make_url_param(url_param** p, char* key, char* value)
{
	(*p) = (url_param*)calloc(1, sizeof(url_param));
	if (!(*p))
	{
		return E_ALLOC_FAILED;
	}
	(*p)->k = str_clone(key);
	(*p)->v = str_clone(value);
	return E_SUCCESS;
}

char* url_param_key(url_param* p)
{
	return p->k;
}

char* url_param_value(url_param* p)
{
	return p->v;
}

void free_url_param(url_param* p)
{
	free(p->k);
	free(p->v);
	free(p);
}

d_err_t make_docker_context_url(docker_context** ctx, const char* url)
{
	(*ctx) = (docker_context*)calloc(1, sizeof(docker_context));
	if (!(*ctx))
	{
		return E_ALLOC_FAILED;
	}
	char* u = (char*)calloc((strlen(url) + 1), sizeof(char));
	if (!u)
	{
		return E_ALLOC_FAILED;
	}
	strcpy(u, url);
	(*ctx)->url = u;
	(*ctx)->api_version = DOCKER_API_VERSION_1_39;
	return E_SUCCESS;
}

d_err_t make_docker_context_default_local(docker_context** ctx) {
#if defined(_WIN32)
	return make_docker_context_url(ctx, DOCKER_DEFAULT_LOCALHOST_URL);
#endif
#if defined(unix) || defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(__MAC__)
	return make_docker_context_url(ctx, DOCKER_DEFAULT_UNIX_SOCKET);
#endif
}

/**
 * Free docker context memory.
 */
d_err_t free_docker_context(docker_context** ctx)
{
	if ((*ctx))
	{
		if ((*ctx)->url)
		{
			free((*ctx)->url);
		}
		free((*ctx));
	}
	return E_SUCCESS;
}

char* build_url(CURL* curl, char* base_url, arraylist* url_params)
{
	if (url_params == NULL)
	{
		return base_url;
	}
	else
	{
		size_t num_params = arraylist_length(url_params);
		if (num_params <= 0)
		{
			return base_url;
		}
		else
		{
			size_t size_toalloc = strlen(base_url) + 1;
			char** allkeys = (char**)calloc(num_params, sizeof(char*));
			char** allvals = (char**)calloc(num_params, sizeof(char*));
			if (allkeys != NULL && allvals != NULL) {
				for (int i = 0; i < num_params; i++)
				{
					url_param* param = (url_param*)arraylist_get(url_params,
						i);
					docker_log_debug("%s=%s\n", param->k, param->v);
					allkeys[i] = curl_easy_escape(curl, param->k, 0);
					allvals[i] = curl_easy_escape(curl, param->v, 0);
				}
				for (int i = 0; i < num_params; i++)
				{
					size_toalloc += strlen(allkeys[i]);
					size_toalloc += strlen(allvals[i]);
					size_toalloc += 2; //for ampersand and equals
				}
				char* url = (char*)calloc(size_toalloc, sizeof(char));
				if (url != NULL) {
					url[0] = '\0';
					strcat(url, base_url);
					if (num_params > 0)
					{
						strcat(url, "?");
						for (int i = 0; i < num_params; i++)
						{
							if (i > 0)
							{
								strcat(url, "&");
							}
							strcat(url, allkeys[i]);
							strcat(url, "=");
							strcat(url, allvals[i]);
						}
					}

					for (int i = 0; i < num_params; i++)
					{
						//free(allkeys[i]);
						//free(allvals[i]);
					}
					free(allkeys);
					free(allvals);

					docker_log_debug("URL Created:\n%s", url);
					return url;
				}
				return NULL;
			}
			return NULL;
		}
	}
}

static size_t write_memory_callback(void* contents, size_t size, size_t nmemb,
	void* userp)
{
	size_t realsize = size * nmemb;
	struct http_response_memory* mem = (struct http_response_memory*) userp;

	char* ptr = realloc(mem->memory, mem->size + realsize + 1);
	if (ptr == NULL)
	{
		/* out of memory! */
		docker_log_debug("not enough memory (realloc returned NULL)");
		return 0;
	}

	mem->memory = ptr;
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;
	//	docker_log_debug("MEMORY-------\n%s\n", mem->memory);

	/** if callback is not null, and current memory contents end with a newline,
	* then flush it to the callback, and empty the memory contents.
	*/
	if (mem->status_callback != NULL && mem->memory)
	{
		char* flush_str = mem->memory + (mem->flush_end * sizeof(char));
		if (flush_str[strlen(flush_str) - 1] == '\n')
		{
			char* msg = str_clone(flush_str);
			mem->status_callback(msg, mem->cbargs, mem->client_cbargs);
			mem->flush_end += strlen(flush_str);
		}
	}
	return realsize;
}

d_err_t set_curl_url(CURL* curl, docker_context* ctx, char* api_url,
	arraylist* url_params)
{
	if (ctx->url != NULL) {
		if (is_unix_socket(ctx->url))
		{
			curl_easy_setopt(curl, CURLOPT_UNIX_SOCKET_PATH, ctx->url);
			const char* local_url = "http://localhost/";
			char* base_url = (char*)calloc((strlen(local_url) + strlen(api_url) + 1),
				sizeof(char));
			if (base_url != NULL) {
				strcpy(base_url, local_url);
				strcat(base_url, api_url);
				char* url = build_url(curl, base_url, url_params);
				curl_easy_setopt(curl, CURLOPT_URL, url);
				free(base_url);
				return E_SUCCESS;
			}
			else {
				return E_ALLOC_FAILED;
			}
		}
		else if (is_http_url(ctx->url))
		{
			char* base_url = (char*)calloc((strlen(ctx->url) + strlen(api_url) + 1),
				sizeof(char));
			if (base_url != NULL) {
				strcpy(base_url, ctx->url);
				strcat(base_url, api_url);
				char* url = build_url(curl, base_url, url_params);
				curl_easy_setopt(curl, CURLOPT_URL, url);
				free(base_url);
				return E_SUCCESS;
			}
			else {
				return E_ALLOC_FAILED;
			}
		}
	}
	return E_INVALID_INPUT;
}

void handle_response(CURLcode res, CURL* curl, docker_result** result,
	struct http_response_memory* chunk, json_object** response)
{
	docker_log_debug("%lu bytes retrieved\n", (unsigned long)chunk->size);
	json_object* response_obj = NULL;
	if (chunk->size > 0)
	{
		response_obj = json_tokener_parse(chunk->memory);
		(*response) = response_obj;
		docker_log_debug("Response = %s",
			json_object_to_json_string(response_obj));
	}
	else
	{
		docker_log_debug("Response = Empty");
	}

	/* Check for errors */
	if (res != CURLE_OK)
	{
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));
		make_docker_result(result, E_CONNECTION_FAILED, -1, NULL,
			NULL);
	}
	else
	{
		long response_code;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		char* effective_url = NULL;
		curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &effective_url);
		if (response_code == 200 || response_code == 201
			|| response_code == 204)
		{
			make_docker_result(result, E_SUCCESS, response_code, effective_url,
				NULL);
		}
		else
		{
			make_docker_result(result, E_INVALID_INPUT, response_code,
				effective_url, "error");
		}
		if ((*result)->http_error_code >= 400)
		{
			char* msg = get_attr_str(response_obj, "message");
			if (msg)
			{
				(*result)->message = msg;
			}
		}
	}
}

d_err_t docker_api_post(docker_context* ctx, docker_result** result,
	char* api_url, arraylist* url_params, const char* post_data,
	struct http_response_memory* chunk, json_object** response)
{
	return docker_api_post_cb(ctx, result, api_url, url_params, post_data,
		chunk, response, NULL, NULL, NULL);
}

d_err_t docker_api_post_buf_cb_w_content_type(docker_context* ctx,
	docker_result** result, char* api_url, arraylist* url_params,
	const void* post_data, size_t post_data_len, struct http_response_memory* chunk,
	json_object** response,
	void (*status_callback)(char* msg, void* cbargs, void* client_cbargs),
	void* cbargs, void* client_cbargs, char* content_type_header)
{
	CURL* curl;
	CURLcode res;
	struct curl_slist* headers = NULL;
	time_t start, end;

	start = time(NULL);

	chunk->memory = malloc(1); /* will be grown as needed by the realloc above */
	chunk->size = 0; /* no data at this point */
	chunk->flush_end = 0;
	chunk->status_callback = status_callback;
	chunk->cbargs = cbargs;
	chunk->client_cbargs = client_cbargs;

	/* get a curl handle */
	curl = curl_easy_init();
	if (curl)
	{
		/* First set the URL that is about to receive our POST. This URL can
		 just as well be a https:// URL if that is what should receive the
		 data. */
		int set_url_err = set_curl_url(curl, ctx, api_url, url_params);
		if (set_url_err)
		{
			return -1;
		}
		headers = curl_slist_append(headers, "Expect:");
		headers = curl_slist_append(headers, content_type_header);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		/* Now specify the POST data */
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, post_data_len);

		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);

		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)chunk);

		/* some servers don't like requests that are made without a user-agent
		 field, so we provide one */
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);

		/* Check for errors */
		handle_response(res, curl, result, chunk, response);
		end = time(NULL);
		if (result != NULL && (*result) != NULL)
		{
			(*result)->method = HTTP_POST_STR;
			if (post_data != NULL)
			{
				(*result)->request_json_str = str_clone(post_data);
			}
			if (chunk->memory != NULL)
			{
				(*result)->response_json_str = str_clone(chunk->memory);
			}
			(*result)->start_time = start;
			(*result)->end_time = end;
		}
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	//TODO check and free chunk
	return E_SUCCESS;
}


d_err_t docker_api_post_cb_w_content_type(docker_context* ctx,
	docker_result** result, char* api_url, arraylist* url_params,
	const char* post_data, struct http_response_memory* chunk,
	json_object** response,
	void (*status_callback)(char* msg, void* cbargs, void* client_cbargs),
	void* cbargs, void* client_cbargs, char* content_type_header)
{
	CURL* curl;
	CURLcode res;
	struct curl_slist* headers = NULL;
	time_t start, end;

	start = time(NULL);

	chunk->memory = malloc(1); /* will be grown as needed by the realloc above */
	chunk->size = 0; /* no data at this point */
	chunk->flush_end = 0;
	chunk->status_callback = status_callback;
	chunk->cbargs = cbargs;
	chunk->client_cbargs = client_cbargs;

	/* get a curl handle */
	curl = curl_easy_init();
	if (curl)
	{
		/* First set the URL that is about to receive our POST. This URL can
		 just as well be a https:// URL if that is what should receive the
		 data. */
		int set_url_err = set_curl_url(curl, ctx, api_url, url_params);
		if (set_url_err)
		{
			return -1;
		}
		headers = curl_slist_append(headers, "Expect:");
		headers = curl_slist_append(headers, content_type_header);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		/* Now specify the POST data */
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L);

		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);

		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)chunk);

		/* some servers don't like requests that are made without a user-agent
		 field, so we provide one */
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);

		/* Check for errors */
		handle_response(res, curl, result, chunk, response);
		end = time(NULL);
		if (result != NULL && (*result) != NULL)
		{
			(*result)->method = HTTP_POST_STR;
			if (post_data != NULL)
			{
				(*result)->request_json_str = str_clone(post_data);
			}
			if (chunk->memory != NULL)
			{
				(*result)->response_json_str = str_clone(chunk->memory);
			}
			(*result)->start_time = start;
			(*result)->end_time = end;
		}
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	//TODO check and free chunk
	return E_SUCCESS;
}

d_err_t docker_api_post_cb(docker_context* ctx, docker_result** result,
	char* api_url, arraylist* url_params, const char* post_data,
	struct http_response_memory* chunk, json_object** response,
	void (*status_callback)(char* msg, void* cbargs, void* client_cbargs),
	void* cbargs, void* client_cbargs)
{
	char* content_type_header = HEADER_JSON;
	docker_api_post_cb_w_content_type(ctx, result, api_url, url_params,
		post_data, chunk, response, status_callback, cbargs, client_cbargs,
		content_type_header);
	return E_SUCCESS;
}

d_err_t docker_api_get(docker_context* ctx, docker_result** result,
	char* api_url, arraylist* url_params,
	struct http_response_memory* chunk, json_object** response)
{
	return docker_api_get_cb(ctx, result, api_url, url_params, chunk, response,
		NULL, NULL, NULL);
}

d_err_t docker_api_get_cb(docker_context* ctx, docker_result** result,
	char* api_url, arraylist* url_params,
	struct http_response_memory* chunk, json_object** response,
	void (*status_callback)(char* msg, void* cbargs, void* client_cbargs),
	void* cbargs, void* client_cbargs)
{
	CURL* curl;
	CURLcode res;
	struct curl_slist* headers = NULL;
	time_t start, end;

	start = time(NULL);

	chunk->memory = malloc(1); /* will be grown as needed by the realloc above */
	chunk->size = 0; /* no data at this point */
	chunk->flush_end = 0;
	chunk->status_callback = status_callback;
	chunk->cbargs = cbargs;
	chunk->client_cbargs = client_cbargs;

	/* get a curl handle */
	curl = curl_easy_init();
	if (curl)
	{
		/* First set the URL that is about to receive our POST. This URL can
		 just as well be a https:// URL if that is what should receive the
		 data. */
		int set_url_err = set_curl_url(curl, ctx, api_url, url_params);
		if (set_url_err)
		{
			return -1;
		}

		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);

		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)chunk);

		/* some servers don't like requests that are made without a user-agent
		 field, so we provide one */
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);

		handle_response(res, curl, result, chunk, response);
		end = time(NULL);
		if (result != NULL && (*result) != NULL)
		{
			(*result)->method = HTTP_GET_STR;
			(*result)->request_json_str = NULL;
			if (chunk->memory != NULL)
			{
				(*result)->response_json_str = str_clone(chunk->memory);
			}
			(*result)->start_time = start;
			(*result)->end_time = end;
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	//TODO check and free chunk
	return E_SUCCESS;
}

d_err_t docker_api_delete(docker_context* ctx, docker_result** result,
	char* api_url, arraylist* url_params,
	struct http_response_memory* chunk, json_object** response)
{
	CURL* curl;
	CURLcode res;
	struct curl_slist* headers = NULL;
	time_t start, end;

	start = time(NULL);

	chunk->memory = malloc(1); /* will be grown as needed by the realloc above */
	chunk->size = 0; /* no data at this point */
	chunk->flush_end = 0;
	chunk->status_callback = NULL;
	chunk->cbargs = NULL;
	chunk->client_cbargs = NULL;

	/* get a curl handle */
	curl = curl_easy_init();
	if (curl)
	{
		/* First set the URL that is about to receive our POST. This URL can
		 just as well be a https:// URL if that is what should receive the
		 data. */
		int set_url_err = set_curl_url(curl, ctx, api_url, url_params);
		if (set_url_err)
		{
			return -1;
		}

		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);

		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)chunk);

		/* some servers don't like requests that are made without a user-agent
		 field, so we provide one */
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);

		handle_response(res, curl, result, chunk, response);
		end = time(NULL);
		if (result != NULL && (*result) != NULL)
		{
			(*result)->method = HTTP_GET_STR;
			(*result)->request_json_str = NULL;
			if (chunk->memory != NULL)
			{
				(*result)->response_json_str = str_clone(chunk->memory);
			}
			(*result)->start_time = start;
			(*result)->end_time = end;
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	//TODO check and free chunk
	return E_SUCCESS;
}

char* create_service_url_id_method(docker_object_type object, const char* id,
	const char* method)
{
	char* object_url = NULL;
	switch (object)
	{
	case CONTAINER:
		object_url = "containers";
		break;
	case IMAGE:
		object_url = "images";
		break;
	case SYSTEM:
		object_url = NULL;
		break;
	case NETWORK:
		object_url = "networks";
		break;
	case VOLUME:
		object_url = "volumes";
		break;
	}
	char* url = NULL;
	if (object_url)
	{
		if (id)
		{
			url = (char*)calloc(
				(strlen(object_url) + strlen(id) + strlen(method) + 3),
				sizeof(char));
			if (url != NULL) {
				sprintf(url, "%s/%s/%s", object_url, id, method);
				docker_log_debug("%s url is %s", method, url);
			}
		}
		else if (method)
		{
			url = (char*)calloc(
				(strlen(object_url) + strlen(method) + 3), sizeof(char));
			if (url != NULL) {
				sprintf(url, "%s/%s", object_url, method);
				docker_log_debug("%s url is %s", method, url);
			}
		}
		else
		{
			url = (char*)calloc((strlen(object_url) + 1), sizeof(char));
			if (url != NULL) {
				sprintf(url, "%s", object_url);
				docker_log_debug("url is %s", url);
			}
		}
	}
	else
	{
		//when there is no object ignore both object and id
		url = (char*)calloc((strlen(method) + 1), sizeof(char));
		if (url != NULL) {
			sprintf(url, "%s", method);
			docker_log_debug("%s url is %s", method, url);
		}
	}
	return url;
}
