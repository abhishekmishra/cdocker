/*
 * clibdocker: docker_system.h
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
#ifndef SRC_DOCKER_SYSTEM_H_
#define SRC_DOCKER_SYSTEM_H_

#include "docker_result.h"
#include "docker_connection_util.h"

#define DOCKER_SYSTEM_GETTER(object, type, name) \
		type docker_ ## object ## _get_ ## name(docker_ ## object* object);

#define DOCKER_SYSTEM_GETTER_ARR_ADD(object, type, name) \
		int docker_ ## object ## _ ## name ## _add(docker_ ## object* object, type data);

#define DOCKER_SYSTEM_GETTER_ARR_LEN(object, name) \
		int docker_ ## object ## _ ## name ## _length(docker_ ## object* object);

#define DOCKER_SYSTEM_GETTER_ARR_GET_IDX(object, type, name) \
		type docker_ ## object ## _ ## name ## _get_idx(docker_ ## object* object, int i);

/**
 * Ping the docker server
 *
 * \param ctx docker context
 * \param result docker result object
 * \return error code
 */
error_t docker_ping(docker_context* ctx, docker_result** result);

typedef struct docker_version_t {
	char* version;
	char* os;
	char* kernel_version;
	char* go_version;
	char* git_commit;
	char* arch;
	char* api_version;
	char* min_api_version;
	char* build_time;
	int experimental;
} docker_version;

/**
 * Construct a new docker_version object.
 */
error_t make_docker_version(docker_version** dv, char* version, char* os,
		char* kernel_version, char* go_version, char* git_commit, char* arch,
		char* api_version, char* min_api_version, char* build_time,
		int experimental);

void free_docker_version(docker_version*dv);

DOCKER_SYSTEM_GETTER(version, char*, version)
DOCKER_SYSTEM_GETTER(version, char*, os)
DOCKER_SYSTEM_GETTER(version, char*, kernel_version)
DOCKER_SYSTEM_GETTER(version, char*, go_version)
DOCKER_SYSTEM_GETTER(version, char*, git_commit)
DOCKER_SYSTEM_GETTER(version, char*, arch)
DOCKER_SYSTEM_GETTER(version, char*, api_version)
DOCKER_SYSTEM_GETTER(version, char*, min_api_version)
DOCKER_SYSTEM_GETTER(version, char*, build_time)
DOCKER_SYSTEM_GETTER(version, int, experimental)

/**
 * Gets the docker version information
 *
 * \param ctx docker context
 * \param result object
 * \param version object to return
 * \return error code.
 */
error_t docker_system_version(docker_context* ctx, docker_result** result,
		docker_version** version);

#endif /* SRC_DOCKER_SYSTEM_H_ */
