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

/**
 * \file docker_volumes.h
 * \brief Docker volumes API
 */

#ifndef SRC_DOCKER_VOLUMES_H_
#define SRC_DOCKER_VOLUMES_H_

#ifdef __cplusplus  
extern "C" {
#endif

#include <stdlib.h>
#include "docker_connection_util.h"
#include "docker_util.h"

typedef struct docker_volume_t {
	time_t created_at;
	char* name;
	char* driver;
	char* mountpoint;
	arraylist* labels; //of pair
	char* scope;
	arraylist* options; //of pair
	arraylist* status; //of pair
} docker_volume;

d_err_t make_docker_volume(docker_volume** volume, time_t created_at,
		char* name, char* driver, char* mountpoint, char* scope);

void free_docker_volume(docker_volume* volume);

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
d_err_t docker_volumes_list(docker_context* ctx, docker_result** result,
		arraylist** volumes, arraylist** warnings,
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
d_err_t docker_volume_create(docker_context* ctx, docker_result** result,
		docker_volume** volume, char* name, char* driver, int num_labels,
		...);

/**
 * Inspect an existing volume.
 *
 * \param ctx the docker context
 * \param result the result object to return
 * \param volume the volume object to return
 * \param name name of the volume to inspect (cannot be NULL)
 * \return error code
 */
d_err_t docker_volume_inspect(docker_context* ctx, docker_result** result,
		docker_volume** volume, char* name);

/**
 * Delete the given volume (identified by name).
 *
 * \param ctx the docker context
 * \param result the result object to return
 * \param name name of the volume to delete
 * \param force force delete if this value is 1
 * \return error code
 */
d_err_t docker_volume_delete(docker_context* ctx, docker_result** result,
		const char* name, int force);

/**
 * Delete unused volumes.
 *
 * \param ctx the docker context
 * \param result the result object to return
 * \param volumes_deleted array_list with names of volumes deleted
 * \param space_reclaimed num bytes freed.
 * \param num_label_filters how many label filters are there
 * \param varargs triples (int filter_not, char* label_name, char* label_value)
 * \return error code
 */
d_err_t docker_volumes_delete_unused(docker_context* ctx,
		docker_result** result, arraylist** volumes_deleted,
		unsigned long* space_reclaimed, int num_label_filters, ...);

#ifdef __cplusplus 
}
#endif

#endif
