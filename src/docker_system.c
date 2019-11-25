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
#include <docker_log.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

#include "docker_system.h"
#include "docker_connection_util.h"

/**
 * Ping the docker server
 *
 * \param ctx docker context
 * \param result docker result object
 * \return error code
 */
d_err_t docker_ping(docker_context* ctx, docker_result** result) {
	char* url = create_service_url_id_method(SYSTEM, NULL, "_ping");

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_get(ctx, result, url, NULL, &chunk, &response_obj);

	if ((*result)->http_error_code != 200) {
		(*result)->message = str_clone("Docker Server not OK.");
		return E_PING_FAILED;
	}

	if (chunk.memory != NULL) {
		free(chunk.memory);
	}
	return E_SUCCESS;
}

/**
 * Construct a new docker_version object.
 */
d_err_t make_docker_version(docker_version** dv, char* version, char* os,
		char* kernel_version, char* go_version, char* git_commit, char* arch,
		char* api_version, char* min_api_version, char* build_time,
		int experimental) {
	(*dv) = (docker_version*) calloc(1, sizeof(docker_version));
	if (!(*dv)) {
		return E_ALLOC_FAILED;
	}
	(*dv)->version = str_clone(version);
	(*dv)->os = str_clone(os);
	(*dv)->kernel_version = str_clone(kernel_version);
	(*dv)->go_version = str_clone(go_version);
	(*dv)->git_commit = str_clone(git_commit);
	(*dv)->arch = str_clone(arch);
	(*dv)->api_version = str_clone(api_version);
	(*dv)->min_api_version = str_clone(min_api_version);
	(*dv)->build_time = str_clone(build_time);
	(*dv)->experimental = experimental;
	return E_SUCCESS;
}

void free_docker_version(docker_version*dv) {
	free(dv->version);
	free(dv->os);
	free(dv->kernel_version);
	free(dv->go_version);
	free(dv->git_commit);
	free(dv->arch);
	free(dv->api_version);
	free(dv->min_api_version);
	free(dv->build_time);
	free(dv);
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

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_get(ctx, result, url, NULL, &chunk, &response_obj);

	if ((*result)->http_error_code >= 200) {
		make_docker_version(version, get_attr_str(response_obj, "Version"),
				get_attr_str(response_obj, "Os"),
				get_attr_str(response_obj, "KernelVersion"),
				get_attr_str(response_obj, "GoVersion"),
				get_attr_str(response_obj, "GitCommit"),
				get_attr_str(response_obj, "Arch"),
				get_attr_str(response_obj, "ApiVersion"),
				get_attr_str(response_obj, "MinAPIVersion"),
				get_attr_str(response_obj, "BuildTime"),
				get_attr_boolean(response_obj, "Experimental"));
	}

	if (chunk.memory != NULL) {
		free(chunk.memory);
	}
	return E_SUCCESS;
}

d_err_t make_docker_info(docker_info** info, unsigned long containers,
		unsigned long containers_running, unsigned long containers_paused,
		unsigned long containers_stopped, unsigned long images) {
	(*info) = (docker_info*) calloc(1, sizeof(docker_info));
	if (!(*info)) {
		return E_ALLOC_FAILED;
	}
	(*info)->containers = containers;
	(*info)->containers_running = containers_running;
	(*info)->containers_paused = containers_paused;
	(*info)->containers_stopped = containers_stopped;
	(*info)->images = images;
	return E_SUCCESS;
}

void free_docker_info(docker_info* info) {
	free(info->name);
	free(info);
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

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_get(ctx, result, url, NULL, &chunk, &response_obj);

	if ((*result)->http_error_code >= 200) {
		make_docker_info(info,
				get_attr_unsigned_long(response_obj, "Containers"),
				get_attr_unsigned_long(response_obj, "ContainersRunning"),
				get_attr_unsigned_long(response_obj, "ContainersPaused"),
				get_attr_unsigned_long(response_obj, "ContainersStopped"),
				get_attr_unsigned_long(response_obj, "Images"));
		(*info)->name = get_attr_str(response_obj, "Name");
		(*info)->ncpu = get_attr_int(response_obj, "NCPU");
		(*info)->memtotal = get_attr_unsigned_long(response_obj, "MemTotal");
	}

	if (chunk.memory != NULL) {
		free(chunk.memory);
	}
	return E_SUCCESS;
}

d_err_t make_docker_event(docker_event** event, char* type, char* action,
		char* actor_id, json_object* actor_attributes, time_t time) {
	(*event) = (docker_event*) calloc(1, sizeof(docker_event));
	if (!(*event)) {
		return E_ALLOC_FAILED;
	}
	(*event)->type = str_clone(type);
	(*event)->action = str_clone(action);
	(*event)->actor_id = str_clone(actor_id);
	(*event)->actor_attributes = actor_attributes;
	(*event)->time = time;
	return E_SUCCESS;
}

void free_docker_event(docker_event* event) {
	free(event->type);
	free(event->action);
	free(event->actor_id);
	free(event->actor_attributes);
	free(event);
}

void parse_events_cb(char* msg, void* cb, void* cbargs) {
	void (*events_cb)(docker_event*, void*) = (void (*)(docker_event*, void*))cb;
	if (msg) {
		if(events_cb) {
			json_object* evt_obj = json_tokener_parse(msg);
			docker_event* evt = (docker_event*)calloc(1, sizeof(docker_event));
			if(evt != NULL) {
				json_object* extractObj;
				char* attr = NULL;
				if (json_object_object_get_ex(evt_obj, "Actor", &extractObj)) {
					json_object* attrs_obj;
					json_object_object_get_ex(extractObj, "Attributes", &attrs_obj);
					make_docker_event(&evt, get_attr_str(evt_obj, "Type"),
							get_attr_str(evt_obj, "Action"),
							get_attr_str(extractObj, "ID"), attrs_obj,
							get_attr_unsigned_long(evt_obj, "time"));
				}
				events_cb(evt, cbargs);
			}
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

	arraylist* params;
	arraylist_new(&params,
			(void (*)(void *)) &free_url_param);
	url_param* p;
	char* start_time_str = (char*) calloc(128, sizeof(char));

	sprintf(start_time_str, "%lu", start_time);
	make_url_param(&p, "since", start_time_str);
	arraylist_add(params, p);

	if (end_time != 0) {
		char* end_time_str = (char*) calloc(128, sizeof(char));
		sprintf(end_time_str, "%lu", end_time);
		make_url_param(&p, "until", end_time_str);
		arraylist_add(params, p);
	}

	json_object *response_obj = NULL;
	struct http_response_memory chunk;

	docker_api_get_cb(ctx, result, url, params, &chunk, &response_obj,
			&parse_events_cb, docker_events_cb, cbargs);

	//cannot use the default response object, as that parses only one object from the response

	arraylist_new(events, (void (*)(void *)) &free_docker_event);
	arraylist* json_arr;
	arraylist_new(&json_arr, &free);

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
					arraylist_add(json_arr, item);
					chunk.memory[i] = '\n';
					start = i;
				}
			}
		}

		size_t num_events = arraylist_length(json_arr);
		docker_log_debug("Read %d items.", num_events);

		arraylist* evtls;
		arraylist_new(&evtls,
				(void (*)(void *)) &free_docker_event);

		for (size_t j = 0; j < num_events; j++) {
			json_object* evt_obj = arraylist_get(json_arr, j);
			docker_event* evt;
			json_object* extractObj;
			char* attr = NULL;
			if (json_object_object_get_ex(evt_obj, "Actor", &extractObj)) {
				json_object* attrs_obj;
				json_object_object_get_ex(extractObj, "Attributes", &attrs_obj);
				make_docker_event(&evt, get_attr_str(evt_obj, "Type"),
						get_attr_str(evt_obj, "Action"),
						get_attr_str(extractObj, "ID"), attrs_obj,
						get_attr_unsigned_long(evt_obj, "time"));
			}
			arraylist_add((*events), evt);
		}
	}

	if (chunk.memory != NULL) {
		free(chunk.memory);
	}
	return E_SUCCESS;
}
