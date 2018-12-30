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

// Docker System Info

typedef struct docker_info_t {
	unsigned long containers;
	unsigned long containers_running;
	unsigned long containers_paused;
	unsigned long containers_stopped;
	unsigned long images;
	char* name;
	int ncpu;
	unsigned long memtotal;
} docker_info;

error_t make_docker_info(docker_info** info, unsigned long containers,
		unsigned long containers_running, unsigned long containers_paused,
		unsigned long containers_stopped, unsigned long images);

void free_docker_info(docker_info* info);

DOCKER_SYSTEM_GETTER(info, unsigned long, containers)
DOCKER_SYSTEM_GETTER(info, unsigned long, containers_running)
DOCKER_SYSTEM_GETTER(info, unsigned long, containers_paused)
DOCKER_SYSTEM_GETTER(info, unsigned long, containers_stopped)
DOCKER_SYSTEM_GETTER(info, unsigned long, images)
DOCKER_SYSTEM_GETTER(info, char*, name)
DOCKER_SYSTEM_GETTER(info, int, ncpu)
DOCKER_SYSTEM_GETTER(info, unsigned long, memtotal)

/**
 * Gets the docker system information
 *
 * \param ctx docker context
 * \param result object
 * \param info object to return
 * \return error code.
 */
error_t docker_system_info(docker_context* ctx, docker_result** result,
		docker_info** info);

// Docker System Events

//{
//
//    "Type": "container",
//    "Action": "create",
//    "Actor":
//
//{
//
//    "ID": "ede54ee1afda366ab42f824e8a5ffd195155d853ceaec74a927f249ea270c743",
//    "Attributes":
//
//        {
//            "com.example.some-label": "some-label-value",
//            "image": "alpine",
//            "name": "my-container"
//        }
//    },
//    "time": 1461943101
//
//}

typedef struct docker_event_t {
	char* type;
	char* action;
	char* actor_id;
	json_object* actor_attributes;
	time_t time;
} docker_event;

error_t make_docker_event(docker_event** event, char* type, char* action,
		char* actor_id, json_object* actor_attributes, time_t time);

void free_docker_event(docker_event* event);

DOCKER_SYSTEM_GETTER(event, char*, type)
DOCKER_SYSTEM_GETTER(event, char*, action)
DOCKER_SYSTEM_GETTER(event, char*, actor_id)
DOCKER_SYSTEM_GETTER(event, json_object*, actor_attributes)
DOCKER_SYSTEM_GETTER(event, time_t, time)

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
		array_list** events, time_t start_time, time_t end_time);

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

#endif /* SRC_DOCKER_SYSTEM_H_ */
