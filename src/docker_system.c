/*
 * clibdocker: docker_system.c
 * Created on: 26-Dec-2018
 *
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
 *
 */
#include "docker_util.h"
#include "docker_system.h"
#include <docker_log.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json_tokener.h>

#include "docker_connection_util.h"

/**
 * Ping the docker server
 *
 * \param ctx docker context
 * \return error code
 */
d_err_t docker_ping(docker_context* ctx) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, SYSTEM, NULL, "_ping") != 0) {
		return E_ALLOC_FAILED;
	}

	json_object *response_obj = NULL;
	d_err_t err = docker_call_exec(ctx, call, &response_obj);

	json_object_put(response_obj);
	free_docker_call(call);
	return E_SUCCESS;
}

/**
 * Gets the docker version information
 *
 * \param ctx docker context
 * \param result object
 * \param version object to return
 * \return error code.
 */
d_err_t docker_system_version(docker_context* ctx, docker_result** result,
		docker_version** version) {
	char* url = create_service_url_id_method(SYSTEM, NULL, "version");
	if(url == NULL) {
		return E_ALLOC_FAILED;
	}

	struct http_response_memory chunk;
	docker_api_get(ctx, result, url, NULL, &chunk, (json_object**) version);

	if (chunk.memory != NULL) {
		free(chunk.memory);
	}
	free(url);
	return E_SUCCESS;
}

/**
 * Gets the docker system information
 *
 * \param ctx docker context
 * \param result object
 * \param info object to return
 * \return error code.
 */
d_err_t docker_system_info(docker_context* ctx, docker_result** result,
		docker_info** info) {
	char* url = create_service_url_id_method(SYSTEM, NULL, "info");
	if (url == NULL) {
		return E_ALLOC_FAILED;
	}

	struct http_response_memory chunk;
	docker_api_get(ctx, result, url, NULL, &chunk, (json_object**)info);

	if (chunk.memory != NULL) {
		free(chunk.memory);
	}
	return E_SUCCESS;
}

void parse_events_cb(char* msg, void* cb, void* cbargs) {
	void (*events_cb)(docker_event*, void*) = (void (*)(docker_event*, void*))cb;
	if (msg) {
		if(events_cb) {
			json_object* evt_obj = json_tokener_parse(msg);
			events_cb(evt_obj, cbargs);
			json_object_put(evt_obj);
		} else {
			docker_log_debug("Message = Empty");
		}
	}
}

/**
 * Get the docker events in a time range.
 *
 * \param ctx the docker context
 * \param result the docker result object to return
 * \param events is an arraylist containing objects of type docker_event
 * \param start_time
 * \param end_time
 * \return error code
 */
d_err_t docker_system_events(docker_context* ctx, docker_result** result,
		arraylist** events, time_t start_time, time_t end_time) {
	if (end_time <= 0) {
		docker_log_warn(
				"This call with end_time %d will never end, and will have no response, use the method with callbacks instead.",
				end_time);
		return E_INVALID_INPUT;
	} else {
		return docker_system_events_cb(ctx, result, NULL, NULL, events,
				start_time, end_time);
	}
}

/**
 * Get the docker events in a time range.
 *
 * \param ctx the docker context
 * \param result the docker result object to return
 * \param docker_events_cb pointer to callback when an event is received.
 * \param cbargs is a pointer to callback arguments
 * \param events is an arraylist containing objects of type docker_event
 * \param start_time
 * \param end_time
 * \return error code
 */
d_err_t docker_system_events_cb(docker_context* ctx, docker_result** result,
		void (*docker_events_cb)(docker_event* evt, void* cbargs), void* cbargs,
		arraylist** events, time_t start_time, time_t end_time) {
	char* url = create_service_url_id_method(SYSTEM, NULL, "events");
	if (url == NULL) {
		return E_ALLOC_FAILED;
	}

	arraylist* params;
	arraylist_new(&params,
			(void (*)(void *)) &free_url_param);
	url_param* p;
	char* start_time_str = (char*) calloc(128, sizeof(char));
	if (start_time_str == NULL) 
	{ 
		return E_ALLOC_FAILED; 
	}
	sprintf(start_time_str, "%lu", start_time);
	make_url_param(&p, "since", start_time_str);
	arraylist_add(params, p);
	free(start_time_str);

	if (end_time != 0) {
		char* end_time_str = (char*) calloc(128, sizeof(char));
		if (end_time_str == NULL)
		{
			return E_ALLOC_FAILED;
		}
		sprintf(end_time_str, "%lu", end_time);
		make_url_param(&p, "until", end_time_str);
		arraylist_add(params, p);
		free(end_time_str);
	}

	json_object *response_obj = NULL;
	struct http_response_memory chunk;

	docker_api_get_cb(ctx, result, url, params, &chunk, &response_obj,
			&parse_events_cb, docker_events_cb, cbargs);

	//cannot use the default response object, as that parses only one object from the response

	arraylist_new(events, (void (*)(void *)) &json_object_put);

	if ((*result)->http_error_code >= 200) {
		if (chunk.memory && strlen(chunk.memory) > 0) {
			size_t len = strlen(chunk.memory);
			size_t start = 0;
			size_t end = 0;
			for (size_t i = 0; i < len; i++) {
				if (chunk.memory[i] == '\n') {
					chunk.memory[i] = '\0';
					json_object* item = json_tokener_parse(
							chunk.memory + start);
					arraylist_add(*events, item);
					chunk.memory[i] = '\n';
					start = i;
				}
			}
		}
	}

	json_object_put(response_obj);
	if (chunk.memory != NULL) {
		free(chunk.memory);
	}
	free(url);
	return E_SUCCESS;
}

d_err_t docker_system_df(docker_context* ctx, docker_result** result,
	docker_df** df) {
	char* url = create_service_url_id_method(SYSTEM, NULL, "system/df");
	if (url == NULL) {
		return E_ALLOC_FAILED;
	}

	struct http_response_memory chunk;
	docker_api_get(ctx, result, url, NULL, &chunk, (json_object**)df);

	if (chunk.memory != NULL) {
		free(chunk.memory);
	}
	return E_SUCCESS;
}
