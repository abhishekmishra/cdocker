/*
 * clibdocker: docker_networks.h
 * Created on: 05-Jan-2019
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

#ifndef SRC_DOCKER_NETWORKS_H_
#define SRC_DOCKER_NETWORKS_H_

#include <stdlib.h>
#include <json-c/arraylist.h>
#include "docker_result.h"
#include "docker_util.h"
#include "docker_connection_util.h"

#define DOCKER_NETWORK_GETTER(object, type, name) \
		type docker_network_ ## object ## _get_ ## name(docker_network_ ## object* object);

#define DOCKER_NETWORK_GETTER_ARR_ADD(object, type, name) \
		int docker_network_ ## object ## _ ## name ## _add(docker_network_ ## object* object, type data);

#define DOCKER_NETWORK_GETTER_ARR_LEN(object, name) \
		int docker_network_ ## object ## _ ## name ## _length(docker_network_ ## object* object);

#define DOCKER_NETWORK_GETTER_ARR_GET_IDX(object, type, name) \
		type docker_network_ ## object ## _ ## name ## _get_idx(docker_network_ ## object* object, int i);

typedef struct docker_network_ipam_t {
	char* driver;
	struct array_list* config; //of ipam_config
	struct array_list* options;
} docker_network_ipam;

error_t make_docker_network_ipam(docker_network_ipam** ipam, char* driver);
void free_docker_network_ipam(docker_network_ipam* ipam);
DOCKER_NETWORK_GETTER(ipam, char*, driver)
DOCKER_NETWORK_GETTER_ARR_ADD(ipam, pair*, config)
DOCKER_NETWORK_GETTER_ARR_LEN(ipam, config)
DOCKER_NETWORK_GETTER_ARR_GET_IDX(ipam, pair*, config)
DOCKER_NETWORK_GETTER_ARR_ADD(ipam, pair*, options)
DOCKER_NETWORK_GETTER_ARR_LEN(ipam, options)
DOCKER_NETWORK_GETTER_ARR_GET_IDX(ipam, pair*, options)

typedef struct docker_network_container_t {
	char* id;
	char* name;
	char* endpoint_id;
	char* mac_address;
	char* ipv4_address;
	char* ipv6_address;
} docker_network_container;

error_t make_docker_network_container(docker_network_container** container,
		char* id, char* name, char* endpoint_id, char* mac_address,
		char* ipv4_address, char* ipv6_address);
void free_docker_network_container(docker_network_container* container);
DOCKER_NETWORK_GETTER(container, char*, id)
DOCKER_NETWORK_GETTER(container, char*, name)
DOCKER_NETWORK_GETTER(container, char*, endpoint_id)
DOCKER_NETWORK_GETTER(container, char*, mac_address)
DOCKER_NETWORK_GETTER(container, char*, ipv4_address)
DOCKER_NETWORK_GETTER(container, char*, ipv6_address)

typedef struct docker_network_item_t {
	char* name;
	char* id;
	time_t created;
	char* scope;
	char* driver;
	int enableIPv6;
	docker_network_ipam* ipam;
	int internal;
	int attachable;
	int ingress;
	struct array_list* containers; // of docker_network_container
	struct array_list* options; //of pair
	struct array_list* labels; //of pair
} docker_network_item;

error_t make_docker_network_item(docker_network_item** network, char* name,
		char* id, time_t created, char* scope, char* driver, int enableIPv6,
		docker_network_ipam* ipam, int internal, int attachable, int ingress);
void free_docker_network_item(docker_network_item* network);
DOCKER_NETWORK_GETTER(item, char*, name)
DOCKER_NETWORK_GETTER(item, char*, id)
DOCKER_NETWORK_GETTER(item, time_t, created)
DOCKER_NETWORK_GETTER(item, char*, scope)
DOCKER_NETWORK_GETTER(item, char*, driver)
DOCKER_NETWORK_GETTER(item, int, enableIPv6)
DOCKER_NETWORK_GETTER(item, docker_network_ipam*, ipam)
DOCKER_NETWORK_GETTER(item, int, internal)
DOCKER_NETWORK_GETTER(item, int, attachable)
DOCKER_NETWORK_GETTER(item, int, ingress)
DOCKER_NETWORK_GETTER_ARR_ADD(item, docker_network_container*, containers)
DOCKER_NETWORK_GETTER_ARR_LEN(item, containers)
DOCKER_NETWORK_GETTER_ARR_GET_IDX(item, docker_network_container*, containers)
DOCKER_NETWORK_GETTER_ARR_ADD(item, pair*, options)
DOCKER_NETWORK_GETTER_ARR_LEN(item, options)
DOCKER_NETWORK_GETTER_ARR_GET_IDX(item, pair*, options)
DOCKER_NETWORK_GETTER_ARR_ADD(item, pair*, labels)
DOCKER_NETWORK_GETTER_ARR_LEN(item, labels)
DOCKER_NETWORK_GETTER_ARR_GET_IDX(item, pair*, labels)

/**
 * List all networks which match the filters given.
 * If all filters are null, then all networks are listed.
 *
 * \param ctx docker context
 * \param result the result object to be returned
 * \param containers the array_list of containers to be returned
 * \param filter_driver
 * \param filter_id
 * \param filter_label
 * \param filter_name
 * \param filter_scope
 * \param filter_type
 * \return error code
 */
error_t docker_networks_list(docker_context* ctx, docker_result** result,
		struct array_list** containers, char* filter_driver, char* filter_id,
		char* filter_label, char* filter_name, char* filter_scope,
		char* filter_type);

#endif /* SRC_DOCKER_NETWORKS_H_ */
