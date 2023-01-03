/*
 *
 * Copyright (c) 2018-2022 Abhishek Mishra
 *
 * This file is part of clibdocker.
 *
 * clibdocker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation, 
 * either version 3 of the License, or (at your option) 
 * any later version.
 *
 * clibdocker is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty 
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public 
 * License along with clibdocker. 
 * If not, see <https://www.gnu.org/licenses/>.
 *
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

/**
 * @brief The Docker Volume object
 * 
 * The docker volume object represents the JSON object, returned
 * as part of a Docker API call.
 * 
 * To access members of the object use the getters of the form
 * \c docker_volume_<member>_get(vol), where vol is the
 * pointer to the volume object.
 * 
 */
typedef json_object										docker_volume;

/**
 * @brief free the docker volume object
 */
#define free_docker_volume(vol)							json_object_put((json_object*) vol)

/**
 * @brief Get the name of the docker volume
 * 
 * @param vol docker volume
 * @return char* name of the volume
 */
#define docker_volume_name_get(vol)						get_attr_str((json_object*)vol, "Name")

/**
 * @brief Get the driver name of the docker volume
 * 
 * @param vol docker volume
 * @return char* driver name of the volume
 */
#define docker_volume_driver_get(vol)					get_attr_str((json_object*)vol, "Driver")

/**
 * @brief Get the mount point of the docker volume
 * 
 * @param vol docker volume
 * @return char* mount point of the volume
 */
#define docker_volume_mountpoint_vol_get(vol)			get_attr_str((json_object*)vol, "Mountpoint")

/**
 * @brief Get the scope of the docker volume
 * 
 * For possible values of scope refer to the docker api docs.
 * 
 * @param vol docker volume
 * @return char* scope of the volume
 */
#define docker_volume_go_scope_get(vol)					get_attr_str((json_object*)vol, "Scope")

/**
 * @brief Get the creation time of the docker volume
 * 
 * @param vol docker volume
 * @return time_t creation time of the volume
 */
#define docker_volume_creation_time_get(vol)			get_attr_time((json_object*)vol, "CreatedAt")

/**
 * @brief Get the volume labels array of the docker volume
 * 
 * The better was to get the volume labels is to use
 * the helper funcitons for iteration over the volume labels.
 * 
 * @param vol docker volume
 * @return json_object* labels of the volume
 */
#define docker_volume_labels_get(vol)					get_attr_json_object((json_object*)vol, "Labels")

/**
 * @brief Iterate over the volume labels with key and val
 * 
 * Iterate over every label, in every iteration,
 * the key and val store the id and value of the label.
 * 
 * @param vol docker volume
 */
#define docker_volume_labels_foreach(vol)				json_object_object_foreach(docker_volume_labels_get(vol), key, val)

/**
 * @brief Get the options array of the docker volume
 * 
 * The better was to get the volume options is to use
 * the helper funcitons for iteration over the volume options.
 * 
 * @param vol docker volume
 * @return json_object* options of the volume
 */
#define docker_volume_options_get(vol)					get_attr_json_object((json_object*)vol, "Options")

/**
 * @brief Iterate over the volume options with key and val
 * 
 * Iterate over every option, in every iteration,
 * the key and val store the id and value of the option.
 * 
 * @param vol docker volume
 */
#define docker_volume_options_foreach(vol)				json_object_object_foreach(docker_volume_options_get(vol), key, val)

/**
 * @brief Represents Docker Volume List in json
 */
typedef json_object										docker_volume_list;

/**
 * @brief Free the docker volume list json object
 * 
 * @param volume_ls the volume list object
 */
#define free_docker_volume_list(volume_ls)				json_object_put(volume_ls)

/**
 * @brief Get the length of the docker volume list
 * 
 * @param volume_ls the volume list object
 * @return size_t length of the volume list
 */
#define docker_volume_list_length(volume_ls)			json_object_array_length(volume_ls)

/**
 * @brief Get the ith value of the docker volume list
 * 
 * @param volume_ls the volume list object
 * @param i index
 * @return docker_volume* ith volume
 */
#define docker_volume_list_get_idx(volume_ls, i)		(docker_volume*) json_object_array_get_idx(volume_ls, i)

/**
 * @brief The Docker Volume Warnings list json object
 *
 * Each warning is a string.
 */
typedef json_object										docker_volume_warnings;

/**
 * @brief Free the docker warnings list
 * 
 * @param warnings docker volume warnings list
 */
#define free_docker_warnings(warnings)					json_object_put(warnings)

/**
 * @brief Get the length of the docker volume warnings list
 * 
 * @param warnings docker volume warnings list 
 * @return size_t volume warnings list length
 */
#define docker_warnings_length(warnings)				json_object_array_length(warnings)

/**
 * @brief Get the ith warning
  *
 * @param warnings docker volume warnings list
 * @param i index
 * @return char* ith warning
 */
#define docker_warnings_get_idx(warnings, i)			(char*) json_object_array_get_idx(warnings, i)

/**
 * Get the list of volumes, matching the filters provided.
 * (Any and all filters can be null/0.)
 *
 * \param ctx the docker context
 * \param volumes the list of docker_volume_item objects
 * \param warnings the list of warnings
 * \param filter_dangling 0 indicates false, anything else is true
 * \param filter_driver driver filter
 * \param filter_label filter label
 * \param filter_name filter name
 * \return error code.
 */
MODULE_API d_err_t docker_volumes_list(docker_context* ctx, 
		docker_volume_list** volumes, docker_volume_warnings** warnings,
		int filter_dangling, char* filter_driver, char* filter_label,
		char* filter_name);

/**
 * Create a new volume by providing name, driver and an optional list of key/value pairs for labels
 *
 * \param ctx the docker context
 * \param volume the volume object to return
 * \param name name of the volume to create (cannot be NULL)
 * \param driver name of the driver to use
 * \param num_labels the number of labels to be attached.
 * \param ... key/values char* key, char* value args pair for each label
 * \return error code
 */
MODULE_API d_err_t docker_volume_create(docker_context* ctx, 
		docker_volume** volume, char* name, char* driver, int num_labels,
		...);

/**
 * Inspect an existing volume.
 *
 * \param ctx the docker context
 * \param volume the volume object to return
 * \param name name of the volume to inspect (cannot be NULL)
 * \return error code
 */
MODULE_API d_err_t docker_volume_inspect(docker_context* ctx, 
		docker_volume** volume, char* name);

/**
 * Delete the given volume (identified by name).
 *
 * \param ctx the docker context
 * \param name name of the volume to delete
 * \param force force delete if this value is 1
 * \return error code
 */
MODULE_API d_err_t docker_volume_delete(docker_context* ctx, 
		const char* name, int force);

/**
 * Delete unused volumes.
 *
 * \param ctx the docker context
 * \param volumes_deleted array_list with names of volumes deleted
 * \param space_reclaimed num bytes freed.
 * \param num_label_filters how many label filters are there
 * \param ... triples (int filter_not, char* label_name, char* label_value)
 * \return error code
 */
MODULE_API d_err_t docker_volumes_delete_unused(docker_context* ctx,
		arraylist** volumes_deleted,
		unsigned long* space_reclaimed, int num_label_filters, ...);

#ifdef __cplusplus 
}
#endif

#endif
