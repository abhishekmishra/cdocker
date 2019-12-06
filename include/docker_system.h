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

/**
 * \file docker_system.h
 * \brief Docker System API
 */

#ifndef SRC_DOCKER_SYSTEM_H_
#define SRC_DOCKER_SYSTEM_H_

#ifdef __cplusplus  
extern "C" {
#endif

#include "docker_common.h"
#include "docker_result.h"
#include "docker_connection_util.h"

#include <json-c/json_object.h>

/**
* Ping the docker server
*
* \param ctx docker context
* \return error code
*/
d_err_t docker_ping(docker_context* ctx);

typedef json_object									docker_version;
#define free_docker_version(version)				json_object_put((json_object*) version)
#define docker_version_version_get(version)			get_attr_str((json_object*)version, "Version")
#define docker_version_os_get(version)				get_attr_str((json_object*)version, "Os")
#define docker_version_kernel_version_get(version)	get_attr_str((json_object*)version, "KernelVersion")
#define docker_version_go_version_get(version)		get_attr_str((json_object*)version, "GoVersion")
#define docker_version_git_commit_get(version)		get_attr_str((json_object*)version, "GitCommit")
#define docker_version_arch_get(version)			get_attr_str((json_object*)version, "Arch")
#define docker_version_api_version_get(version)		get_attr_str((json_object*)version, "ApiVersion")
#define docker_version_min_api_version_get(version) get_attr_str((json_object*)version, "MinAPIVersion")
#define docker_version_build_time_get(version)		get_attr_str((json_object*)version, "BuildTime")
#define docker_version_experimental_get(version)	get_attr_boolean((json_object*)version, "Experimental")

/**
 * Gets the docker version information
 *
 * \param ctx docker context
 * \param result object
 * \param version object to return
 * \return error code.
 */
MODULE_API d_err_t docker_system_version(docker_context* ctx, docker_result** result,
		docker_version** version);

typedef json_object									docker_info;
#define free_docker_info(info)						json_object_put((json_object*) info)
#define docker_info_containers_get(info)			get_attr_unsigned_long((json_object*)info, "Containers")
#define docker_info_containers_running_get(info)	get_attr_unsigned_long((json_object*)info, "ContainersRunning")
#define docker_info_containers_paused_get(info)		get_attr_unsigned_long((json_object*)info, "ContainersPaused")
#define docker_info_containers_stopped_get(info)	get_attr_unsigned_long((json_object*)info, "ContainersStopped")
#define docker_info_images_get(info)				get_attr_unsigned_long((json_object*)info, "Images")
#define docker_info_name_get(info)					get_attr_str((json_object*)info, "Name")
#define docker_info_ncpu_get(info)					get_attr_int((json_object*)info, "NCPU")
#define docker_info_mem_total_get(info)				get_attr_unsigned_long((json_object*)info, "MemTotal")

/**
 * Gets the docker system information
 *
 * \param ctx docker context
 * \param result object
 * \param info object to return
 * \return error code.
 */
d_err_t docker_system_info(docker_context* ctx, docker_result** result,
		docker_info** info);

// Docker System Events API

typedef arraylist									docker_event_list;
#define free_docker_event_list(event_ls)			arraylist_free(event_ls)
#define docker_event_list_length(event_ls)			arraylist_length(event_ls)
#define docker_event_list_get_idx(event_ls, i)			arraylist_get(event_ls, i)

typedef json_object									docker_event;
#define free_docker_event(event)					json_object_put((json_object*) event)
#define docker_event_type_get(event)				get_attr_str((json_object*)event, "Type")
#define docker_event_action_get(event)				get_attr_str((json_object*)event, "Action")
#define docker_event_time_get(event)				get_attr_unsigned_long((json_object*)event, "time")
#define docker_event_actor_id_get(event)			get_attr_str(get_attr_json_object((json_object*)event, "Actor"), "ID")
#define docker_event_attributes_get(event)			get_attr_json_object(get_attr_json_object((json_object*)event, "Actor"), "Attributes")
#define docker_event_attributes_foreach(event)		json_object_object_foreach(docker_event_attributes_get(event), key, val)

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
d_err_t docker_system_events(docker_context* ctx, docker_result** result,
		arraylist** events, time_t start_time, time_t end_time);

/**
 * Get the docker events in a time range.
 *
 * \param ctx the docker context
 * \param result the docker result object to return
 * \param docker_events_cb pointer to callback when an event is received.
 * \param cbargs is a pointer to callback arguments
 * \param events is an array_list containing objects of type docker_event
 * \param start_time
 * \param end_time
 * \return error code
 */
d_err_t docker_system_events_cb(docker_context* ctx, docker_result** result,
		void (*docker_events_cb)(docker_event* evt, void* cbargs), void* cbargs,
		arraylist** events, time_t start_time, time_t end_time);

//Docker df

/**
 * Gets the docker version information
 *
 * \param ctx docker context
 * \param result object
 * \param version object to return
 * \return error code.
 */
MODULE_API d_err_t docker_system_version(docker_context* ctx, docker_result** result,
	docker_version** version);

typedef json_object										docker_df;
typedef json_object										docker_df_image;
typedef json_object										docker_df_container;
typedef json_object										docker_df_volume;
typedef json_object										docker_df_volume_usage_data;

#define docker_df_image_id_get(img)						get_attr_str((json_object*)img, "Id")
#define docker_df_image_parent_id_get(img)				get_attr_str((json_object*)img, "ParentId")
#define docker_df_image_created_get(img)				get_attr_unsigned_long((json_object*)img, "Created")
#define docker_df_image_size_get(img)					get_attr_unsigned_long((json_object*)img, "Size")
#define docker_df_image_shared_size_get(img)			get_attr_unsigned_long((json_object*)img, "SharedSize")
#define docker_df_image_virtual_size_get(img)			get_attr_unsigned_long((json_object*)img, "VirtualSize")
#define docker_df_image_containers_get(img)				get_attr_unsigned_long((json_object*)img, "Containers")

#define docker_df_image_repo_tags_get(img)				get_attr_json_object((json_object*)img, "RepoTags")
#define docker_df_image_repo_tags_length(img)			json_object_array_length(docker_df_image_repo_tags_get(img))
#define docker_df_image_repo_tags_get_idx(img, i)		(const char*)json_object_array_get_idx(docker_df_image_repo_tags_get(img), i)

#define docker_df_image_repo_digests_get(img)			get_attr_json_object((json_object*)img, "RepoDigests")
#define docker_df_image_repo_digests_length(img)		json_object_array_length(docker_df_image_repo_digests_get(img))
#define docker_df_image_repo_digests_get_idx(img, i)	(const char*)json_object_array_get_idx(docker_df_image_repo_digests_get(img), i)

#define docker_df_container_id_get(ctr)					get_attr_str((json_object*)ctr, "Id")
#define docker_df_container_image_get(ctr)				get_attr_str((json_object*)ctr, "Image")
#define docker_df_container_image_id_get(ctr)			get_attr_str((json_object*)ctr, "ImageID")
#define docker_df_container_command_get(ctr)			get_attr_str((json_object*)ctr, "Command")
#define docker_df_container_created_get(ctr)			get_attr_unsigned_long((json_object*)ctr, "Created")
#define docker_df_container_size_rootfs_get(ctr)		get_attr_unsigned_long((json_object*)ctr, "SizeRootFs")
#define docker_df_container_state_get(ctr)				get_attr_str((json_object*)ctr, "State")
#define docker_df_container_status_get(ctr)				get_attr_str((json_object*)ctr, "Status")

#define docker_df_volume_name_get(vol)					get_attr_str((json_object*)vol, "Name")
#define docker_df_volume_driver_get(vol)				get_attr_str((json_object*)vol, "Driver")
#define docker_df_volume_mountpoint_get(vol)			get_attr_str((json_object*)vol, "Mountpoint")
#define docker_df_volume_scope_get(vol)					get_attr_str((json_object*)vol, "Scope")
#define docker_df_volume_usage_data_get(vol)			(docker_df_volume_usage_data*)get_attr_json_object((json_object*)vol, "UsageData")

#define docker_df_volume_usage_data_size_get(ud)		get_attr_unsigned_long((json_object*)ud, "Size")
#define docker_df_volume_usage_data_ref_count_get(ud)	get_attr_unsigned_long((json_object*)ud, "RefCount")

#define docker_df_container_names_get(ctr)				get_attr_json_object((json_object*)ctr, "Names")
#define docker_df_container_names_length(ctr)			json_object_array_length(docker_df_container_names_get(ctr))
#define docker_df_container_names_get_idx(ctr, i)		(const char*)json_object_array_get_idx(docker_df_container_names_get(ctr), i)

#define docker_df_container_mounts_get(ctr)				get_attr_json_object((json_object*)ctr, "Mounts")
#define docker_df_container_mounts_length(ctr)			json_object_array_length(docker_df_container_mounts_get(ctr))
#define docker_df_container_mounts_get_idx(ctr, i)		(const char*)json_object_array_get_idx(docker_df_container_mounts_get(ctr), i)

#define free_docker_df(df)								json_object_put((json_object*) df)
#define docker_df_layers_size_get(df)					get_attr_unsigned_long((json_object*)df, "LayersSize")

#define docker_df_images_get(df)						get_attr_json_object((json_object*)df, "Images")
#define docker_df_images_length(df)						json_object_array_length(docker_df_images_get(df))
#define docker_df_images_get_idx(df, i)					(docker_df_image*)json_object_array_get_idx(docker_df_images_get(df), i)

#define docker_df_containers_get(df)					get_attr_json_object((json_object*)df, "Containers")
#define docker_df_containers_length(df)					json_object_array_length(docker_df_containers_get(df))
#define docker_df_containers_get_idx(df, i)				(docker_df_container*)json_object_array_get_idx(docker_df_containers_get(df), i)

#define docker_df_volumes_get(df)						get_attr_json_object((json_object*)df, "Volumes")
#define docker_df_volumes_length(df)					json_object_array_length(docker_df_volumes_get(df))
#define docker_df_volumes_get_idx(df, i)				(docker_df_volume*)json_object_array_get_idx(docker_df_volumes_get(df), i)

//TODO docker df
/**
 * Gets the docker usage data
 *
 * \param ctx docker context
 * \param result object
 * \param docker_df object to return
 * \return error code.
 */
MODULE_API d_err_t docker_system_df(docker_context* ctx, docker_result** result,
	docker_df** df);

#ifdef __cplusplus 
}
#endif

#endif /* SRC_DOCKER_SYSTEM_H_ */
