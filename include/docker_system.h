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
 * \param result docker result object
 * \return error code
 */
MODULE_API d_err_t docker_ping(docker_context* ctx, docker_result** result);

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
#define docker_event_list_get(event_ls, i)			arraylist_get(event_ls, i)

typedef json_object									docker_event;
#define free_docker_event(event)					json_object_put((json_object*) event)
#define docker_event_type_get(event)				get_attr_str((json_object*)event, "Type")
#define docker_event_action_get(event)				get_attr_str((json_object*)event, "Action")
#define docker_event_time_get(event)				get_attr_unsigned_long((json_object*)event, "time")
#define docker_event_actor_id_get(event)			get_attr_str(get_json_object((json_object*)event, "Actor"), "ID")
#define docker_event_attributes_get(event)			get_json_object(get_json_object((json_object*)event, "Actor"), "Attributes")
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

//{
//  "LayersSize": 1092588,
//  "Images": [
//    {
//      "Id": "sha256:2b8fd9751c4c0f5dd266fcae00707e67a2545ef34f9a29354585f93dac906749",
//      "ParentId": "",
//      "RepoTags": [
//        "busybox:latest"
//      ],
//      "RepoDigests": [
//        "busybox@sha256:a59906e33509d14c036c8678d687bd4eec81ed7c4b8ce907b888c607f6a1e0e6"
//      ],
//      "Created": 1466724217,
//      "Size": 1092588,
//      "SharedSize": 0,
//      "VirtualSize": 1092588,
//      "Labels": {},
//      "Containers": 1
//    }
//  ],
//  "Containers": [
//    {
//      "Id": "e575172ed11dc01bfce087fb27bee502db149e1a0fad7c296ad300bbff178148",
//      "Names": [
//        "/top"
//      ],
//      "Image": "busybox",
//      "ImageID": "sha256:2b8fd9751c4c0f5dd266fcae00707e67a2545ef34f9a29354585f93dac906749",
//      "Command": "top",
//      "Created": 1472592424,
//      "Ports": [],
//      "SizeRootFs": 1092588,
//      "Labels": {},
//      "State": "exited",
//      "Status": "Exited (0) 56 minutes ago",
//      "HostConfig": {
//        "NetworkMode": "default"
//      },
//      "NetworkSettings": {
//        "Networks": {
//          "bridge": {
//            "IPAMConfig": null,
//            "Links": null,
//            "Aliases": null,
//            "NetworkID": "d687bc59335f0e5c9ee8193e5612e8aee000c8c62ea170cfb99c098f95899d92",
//            "EndpointID": "8ed5115aeaad9abb174f68dcf135b49f11daf597678315231a32ca28441dec6a",
//            "Gateway": "172.18.0.1",
//            "IPAddress": "172.18.0.2",
//            "IPPrefixLen": 16,
//            "IPv6Gateway": "",
//            "GlobalIPv6Address": "",
//            "GlobalIPv6PrefixLen": 0,
//            "MacAddress": "02:42:ac:12:00:02"
//          }
//        }
//      },
//      "Mounts": []
//    }
//  ],
//  "Volumes": [
//    {
//      "Name": "my-volume",
//      "Driver": "local",
//      "Mountpoint": "/var/lib/docker/volumes/my-volume/_data",
//      "Labels": null,
//      "Scope": "local",
//      "Options": null,
//      "UsageData": {
//        "Size": 10920104,
//        "RefCount": 2
//      }
//    }
//  ]
//}

//TODO docker df

#ifdef __cplusplus 
}
#endif

#endif /* SRC_DOCKER_SYSTEM_H_ */
