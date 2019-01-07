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
	struct array_list* status; //of pair
} docker_volume_item;

error_t make_docker_volume_item(docker_volume_item** volume, time_t created_at,
		char* name, char* driver, char* mountpoint, char* scope);

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

DOCKER_VOLUME_GETTER_ARR_ADD(item, pair*, status)
DOCKER_VOLUME_GETTER_ARR_LEN(item, status)
DOCKER_VOLUME_GETTER_ARR_GET_IDX(item, pair*, status)

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
		int filter_dangling, char* filter_driver, char* filter_label,
		char* filter_name);

/**
 * Create a new volume by providing name, driver and an optional list of key/value pairs for labels
 *
 * \param ctx the docker context
 * \param result the result object to return
 * \param volume the volume object to return
 * \param name name of the volume to create (cannot be NULL)
 * \param driver name of the driver to use
 * \param num_labels the number of labels to be attached.
 * \param key/values char* key, char* value args pair for each label
 * \return error code
 */
error_t docker_volume_create(docker_context* ctx, docker_result** result,
		docker_volume_item** volume, char* name, char* driver, int num_labels,
		...);

/**
 * Delete the given volume (identified by name).
 *
 * \param ctx the docker context
 * \param result the result object to return
 * \param name name of the volume to delete
 * \param force force delete if this value is 1
 * \return error code
 */
error_t docker_volume_delete(docker_context* ctx, docker_result** result,
		const char* name, int force);

/**
 * Delete unused volumes.
 *
 * \param ctx the docker context
 * \param result the result object to return
 * \param volumes_deleted array_list with names of volumes deleted
 * \param space_reclaimed num bytes freed.
 * \param filter_not whether the filter is NOT
 * \param label_name optional filter label
 * \param label_value optional filter value
 * \return error code
 */
error_t docker_volumes_delete_unused(docker_context* ctx,
		docker_result** result, struct array_list** volumes_deleted,
		unsigned long* space_reclaimed, int filter_not, char* label_name, char* label_value);
#endif
