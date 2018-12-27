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
	(*dv) = (docker_version*)malloc(sizeof(docker_version));
	if(!(*dv)) {
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
		make_docker_version(version,
		get_attr_str(response_obj, "Version"),
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
