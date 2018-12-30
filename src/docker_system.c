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
#include <stdlib.h>
#include <string.h>
#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

#include "docker_system.h"
#include "docker_util.h"
#include "docker_connection_util.h"
#include "log.h"

#define DOCKER_SYSTEM_GETTER_IMPL(object, type, name) \
	type docker_ ## object ## _get_ ## name(docker_ ## object* object) { \
		return object->name; \
	} \


#define DOCKER_SYSTEM_GETTER_ARR_ADD_IMPL(object, type, name) \
	int docker_ ## object ## _ ## name ## _add(docker_ ## object* object, type data) { \
		return array_list_add(object->name, (void*) data); \
	} \

#define DOCKER_SYSTEM_GETTER_ARR_LEN_IMPL(object, name) \
	int docker_ ## object ## _ ## name ##_length(docker_ ## object* object) { \
		return array_list_length(object->name); \
	} \

#define DOCKER_SYSTEM_GETTER_ARR_GET_IDX_IMPL(object, type, name) \
	type docker_ ## object ## _ ## name ## _get_idx(docker_ ## object* object, int i) { \
		return (type) array_list_get_idx(object->name, i); \
	} \


/**
 * Ping the docker server
 *
 * \param ctx docker context
 * \param result docker result object
 * \return error code
 */
error_t docker_ping(docker_context* ctx, docker_result** result) {
	char* url = create_service_url_id_method(SYSTEM, NULL, "_ping");

	json_object *response_obj = NULL;
	struct MemoryStruct chunk;
	docker_api_get(ctx, result, url, NULL, &chunk, &response_obj);

	if ((*result)->http_error_code != 200) {
		(*result)->message = make_defensive_copy("Docker Server not OK.");
		return E_PING_FAILED;
	}

	return E_SUCCESS;
}

/**
 * Construct a new docker_version object.
 */
error_t make_docker_version(docker_version** dv, char* version, char* os,
		char* kernel_version, char* go_version, char* git_commit, char* arch,
		char* api_version, char* min_api_version, char* build_time,
		int experimental) {
	(*dv) = (docker_version*) malloc(sizeof(docker_version));
	if (!(*dv)) {
		return E_ALLOC_FAILED;
	}
	(*dv)->version = make_defensive_copy(version);
	(*dv)->os = make_defensive_copy(os);
	(*dv)->kernel_version = make_defensive_copy(kernel_version);
	(*dv)->go_version = make_defensive_copy(go_version);
	(*dv)->git_commit = make_defensive_copy(git_commit);
	(*dv)->arch = make_defensive_copy(arch);
	(*dv)->api_version = make_defensive_copy(api_version);
	(*dv)->min_api_version = make_defensive_copy(min_api_version);
	(*dv)->build_time = make_defensive_copy(build_time);
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

DOCKER_SYSTEM_GETTER_IMPL(version, char*, version)
DOCKER_SYSTEM_GETTER_IMPL(version, char*, os)
DOCKER_SYSTEM_GETTER_IMPL(version, char*, kernel_version)
DOCKER_SYSTEM_GETTER_IMPL(version, char*, go_version)
DOCKER_SYSTEM_GETTER_IMPL(version, char*, git_commit)
DOCKER_SYSTEM_GETTER_IMPL(version, char*, arch)
DOCKER_SYSTEM_GETTER_IMPL(version, char*, api_version)
DOCKER_SYSTEM_GETTER_IMPL(version, char*, min_api_version)
DOCKER_SYSTEM_GETTER_IMPL(version, char*, build_time)
DOCKER_SYSTEM_GETTER_IMPL(version, int, experimental)

/**
 * Gets the docker version information
 *
 * \param ctx docker context
 * \param result object
 * \param version object to return
 * \return error code.
 */
error_t docker_system_version(docker_context* ctx, docker_result** result,
		docker_version** version) {
	char* url = create_service_url_id_method(SYSTEM, NULL, "version");

	json_object *response_obj = NULL;
	struct MemoryStruct chunk;
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

	return E_SUCCESS;

}

error_t make_docker_info(docker_info** info, unsigned long containers,
		unsigned long containers_running, unsigned long containers_paused,
		unsigned long containers_stopped, unsigned long images) {
	(*info) = (docker_info*) malloc(sizeof(docker_info));
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
	free(info);
}

DOCKER_SYSTEM_GETTER_IMPL(info, unsigned long, containers)
DOCKER_SYSTEM_GETTER_IMPL(info, unsigned long, containers_running)
DOCKER_SYSTEM_GETTER_IMPL(info, unsigned long, containers_paused)
DOCKER_SYSTEM_GETTER_IMPL(info, unsigned long, containers_stopped)
DOCKER_SYSTEM_GETTER_IMPL(info, unsigned long, images)

/**
 * Gets the docker system information
 *
 * \param ctx docker context
 * \param result object
 * \param info object to return
 * \return error code.
 */
error_t docker_system_info(docker_context* ctx, docker_result** result,
		docker_info** info) {
	char* url = create_service_url_id_method(SYSTEM, NULL, "info");

	json_object *response_obj = NULL;
	struct MemoryStruct chunk;
	docker_api_get(ctx, result, url, NULL, &chunk, &response_obj);

	if ((*result)->http_error_code >= 200) {
		make_docker_info(info,
				get_attr_unsigned_long(response_obj, "Containers"),
				get_attr_unsigned_long(response_obj, "ContainersRunning"),
				get_attr_unsigned_long(response_obj, "ContainersPaused"),
				get_attr_unsigned_long(response_obj, "ContainersStopped"),
				get_attr_unsigned_long(response_obj, "Images"));
	}

	return E_SUCCESS;

}

error_t make_docker_event(docker_event** event, char* type, char* action,
		char* actor_id, json_object* actor_attributes, time_t time) {
	(*event) = (docker_event*) malloc(sizeof(docker_event));
	if (!(*event)) {
		return E_ALLOC_FAILED;
	}
	(*event)->type = make_defensive_copy(type);
	(*event)->action = make_defensive_copy(action);
	(*event)->actor_id = make_defensive_copy(actor_id);
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

DOCKER_SYSTEM_GETTER_IMPL(event, char*, type)
DOCKER_SYSTEM_GETTER_IMPL(event, char*, action)
DOCKER_SYSTEM_GETTER_IMPL(event, char*, actor_id)
DOCKER_SYSTEM_GETTER_IMPL(event, json_object*, actor_attributes)
DOCKER_SYSTEM_GETTER_IMPL(event, time_t, time)

/**
 * Get the docker events in a time range.
 *
 * \param ctx the docker context
 * \param result the docker result object to return
 * \param events is an array_list containing objects of type docker_event
 * \param start_time
 * \param end_time
 * \return error code
 */
error_t docker_system_events(docker_context* ctx, docker_result** result,
		array_list** events, time_t start_time, time_t end_time) {
	char* url = create_service_url_id_method(SYSTEM, NULL, "events");

	struct array_list* params = array_list_new(
			(void (*)(void *)) &free_url_param);
	url_param* p;
	char* start_time_str = (char*) calloc(128, sizeof(char));

	sprintf(start_time_str, "%lu", start_time);
	make_url_param(&p, "since", start_time_str);
	array_list_add(params, p);

	if (end_time != 0) {
		char* end_time_str = (char*) calloc(128, sizeof(char));
		sprintf(end_time_str, "%lu", end_time);
		make_url_param(&p, "until", end_time_str);
		array_list_add(params, p);
	}

	json_object *response_obj = NULL;
	struct MemoryStruct chunk;

	docker_api_stream(ctx, result, url, params, &chunk, &response_obj);

	//cannot use the default response object, as that parses only one object from the response

	(*events) = array_list_new((void (*)(void *))&free_docker_event);
	array_list* json_arr = array_list_new(&free);

	if ((*result)->http_error_code >= 200) {
		if (chunk.memory && strlen(chunk.memory) > 0) {
			int len = strlen(chunk.memory);
			int start = 0;
			int end = 0;
			for (int i = 0; i < len; i++) {
				if (chunk.memory[i] == '\n') {
					chunk.memory[i] = '\0';
					json_object* item = json_tokener_parse(
							chunk.memory + start);
					array_list_add(json_arr, item);
					chunk.memory[i] = '\n';
					start = i;
				}
			}
		}

		int num_events = array_list_length(json_arr);
		docker_log_debug("Read %d items.", num_events);

		array_list* evtls = array_list_new(
				(void (*)(void *)) &free_docker_event);

		for (int j = 0; j < num_events; j++) {
			json_object* evt_obj = array_list_get_idx(json_arr, j);
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
			array_list_add((*events), evt);
		}
	}

	return E_SUCCESS;

}
