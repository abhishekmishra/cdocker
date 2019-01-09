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

#ifdef __cplusplus  
extern "C" {
#endif

#include <stdlib.h>
#include <json-c/arraylist.h>
#include "docker_result.h"
#include "docker_util.h"
#include "docker_connection_util.h"

typedef struct docker_network_ipam_config_t {
	char* subnet;
	char* gateway;
} docker_network_ipam_config;

error_t make_docker_network_ipam_config(docker_network_ipam_config** config, char* subnet, char* gateway);
void free_docker_network_ipam_config(docker_network_ipam_config* config);
DOCKER_GETTER(network_ipam_config, char*, subnet)
DOCKER_GETTER(network_ipam_config, char*, gateway)

typedef struct docker_network_ipam_t {
	char* driver;
	struct array_list* config; //of docker_network_ipam_config
	struct array_list* options;
} docker_network_ipam;

error_t make_docker_network_ipam(docker_network_ipam** ipam, char* driver);
void free_docker_network_ipam(docker_network_ipam* ipam);
DOCKER_GETTER(network_ipam, char*, driver)
DOCKER_GETTER_ARR_ADD(network_ipam, docker_network_ipam_config*, config)
DOCKER_GETTER_ARR_LEN(network_ipam, config)
DOCKER_GETTER_ARR_GET_IDX(network_ipam, docker_network_ipam_config*, config)
DOCKER_GETTER_ARR_ADD(network_ipam, pair*, options)
DOCKER_GETTER_ARR_LEN(network_ipam, options)
DOCKER_GETTER_ARR_GET_IDX(network_ipam, pair*, options)

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
DOCKER_GETTER(network_container, char*, id)
DOCKER_GETTER(network_container, char*, name)
DOCKER_GETTER(network_container, char*, endpoint_id)
DOCKER_GETTER(network_container, char*, mac_address)
DOCKER_GETTER(network_container, char*, ipv4_address)
DOCKER_GETTER(network_container, char*, ipv6_address)

typedef struct docker_network_t {
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
} docker_network;

error_t make_docker_network(docker_network** network, char* name,
		char* id, time_t created, char* scope, char* driver, int enableIPv6,
		docker_network_ipam* ipam, int internal, int attachable, int ingress);
void free_docker_network(docker_network* network);
DOCKER_GETTER(network, char*, name)
DOCKER_GETTER(network, char*, id)
DOCKER_GETTER(network, time_t, created)
DOCKER_GETTER(network, char*, scope)
DOCKER_GETTER(network, char*, driver)
DOCKER_GETTER(network, int, enableIPv6)
DOCKER_GETTER(network, docker_network_ipam*, ipam)
DOCKER_GETTER(network, int, internal)
DOCKER_GETTER(network, int, attachable)
DOCKER_GETTER(network, int, ingress)
DOCKER_GETTER_ARR_ADD(network, docker_network_container*, containers)
DOCKER_GETTER_ARR_LEN(network, containers)
DOCKER_GETTER_ARR_GET_IDX(network, docker_network_container*, containers)
DOCKER_GETTER_ARR_ADD(network, pair*, options)
DOCKER_GETTER_ARR_LEN(network, options)
DOCKER_GETTER_ARR_GET_IDX(network, pair*, options)
DOCKER_GETTER_ARR_ADD(network, pair*, labels)
DOCKER_GETTER_ARR_LEN(network, labels)
DOCKER_GETTER_ARR_GET_IDX(network, pair*, labels)

/**
 * List all networks which match the filters given.
 * If all filters are null, then all networks are listed.
 *
 * \param ctx docker context
 * \param result the result object to be returned
 * \param networks the array_list of networks to be returned
 * \param filter_driver
 * \param filter_id
 * \param filter_label
 * \param filter_name
 * \param filter_scope
 * \param filter_type
 * \return error code
 */
error_t docker_networks_list(docker_context* ctx, docker_result** result,
		struct array_list** networks, char* filter_driver, char* filter_id,
		char* filter_label, char* filter_name, char* filter_scope,
		char* filter_type);

/**
 * Inspect details of a network looked up by name or id.
 *
 * \param ctx docker context
 * \param result the result object to be returned
 * \param net details of the network returned
 * \param id_or_name id or name of the network to be looked up
 * \param verbose whether inspect output is verbose (0 means false, true otherwise)
 * \param scope filter by one of swarm, global, or local
 * \return error code
 */
error_t docker_network_inspect(docker_context* ctx, docker_result** result,
		docker_network** net, char* id_or_name, int verbose, char* scope);

#ifdef __cplusplus 
}
#endif

#endif /* SRC_DOCKER_NETWORKS_H_ */
