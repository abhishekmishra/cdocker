/*
 * clibdocker: docker_volumes.h
 * Created on: 06-Jan-2019
 *
 * clibdocker
 * Copyright (C) 2018 Abhishek Mishra <abhishekmishra3@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SRC_DOCKER_VOLUMES_H_
#define SRC_DOCKER_VOLUMES_H_

#include <stdlib.h>
#include <json-c/arraylist.h>
#include "docker_connection_util.h"
#include "docker_util.h"

#define DOCKER_VOLUME_GETTER(object, type, name) \
		type docker_volume_ ## object ## _get_ ## name(docker_volume_ ## object* object);

#define DOCKER_VOLUME_GETTER_ARR_ADD(object, type, name) \
		int docker_volume_ ## object ## _ ## name ## _add(docker_volume_ ## object* object, type data);

#define DOCKER_VOLUME_GETTER_ARR_LEN(object, name) \
		int docker_volume_ ## object ## _ ## name ## _length(docker_volume_ ## object* object);

#define DOCKER_VOLUME_GETTER_ARR_GET_IDX(object, type, name) \
		type docker_volume_ ## object ## _ ## name ## _get_idx(docker_volume_ ## object* object, int i);

typedef struct docker_volume_item_t {
	time_t created_at;
	char* name;
	char* driver;
	char* mountpoint;
	struct array_list* labels; //of pair
	char* scope;
	struct array_list* options; //of pair
} docker_volume_item;

error_t make_docker_volume_item(docker_volume_item** volume, time_t created_at, char* name,
	char* driver, char* mountpoint, char* scope);

void free_docker_volume_item(docker_volume_item* volume);

DOCKER_VOLUME_GETTER(item, time_t, created_at)
DOCKER_VOLUME_GETTER(item, char*, name)
DOCKER_VOLUME_GETTER(item, char*, driver)
DOCKER_VOLUME_GETTER(item, char*, mountpoint)
DOCKER_VOLUME_GETTER(item, char*, scope)

DOCKER_VOLUME_GETTER_ARR_ADD(item, pair*, labels)
DOCKER_VOLUME_GETTER_ARR_LEN(item, labels)
DOCKER_VOLUME_GETTER_ARR_GET_IDX(item, pair*, labels)

DOCKER_VOLUME_GETTER_ARR_ADD(item, pair*, options)
DOCKER_VOLUME_GETTER_ARR_LEN(item, options)
DOCKER_VOLUME_GETTER_ARR_GET_IDX(item, pair*, options)

/**
* Get the list of volumes, matching the filters provided.
* (Any and all filters can be null/0.)
* 
* \param ctx the docker context
* \param result the result object to be returned.
* \param volumes the list of docker_volume_item objects
* \param warnings the list of warnings
* \param filter_dangling 0 indicates false, anything else is true
* \param filter_driver driver filter
* \param filter_label
* \param filter_name
* \return error code.
*/
error_t docker_volumes_list(docker_context* ctx, docker_result** result, 
	struct array_list** volumes, struct array_list** warnings, 
	int filter_dangling, char* filter_driver, char* filter_label, char* filter_name);
#endif