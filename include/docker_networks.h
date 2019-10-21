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

/**
 * \file docker_networks.h
 * \brief Docker Networks API
 */

#ifndef SRC_DOCKER_NETWORKS_H_
#define SRC_DOCKER_NETWORKS_H_

#ifdef __cplusplus  
extern "C" {
#endif

#include <stdlib.h>
#include "docker_result.h"
#include "docker_util.h"
#include "docker_connection_util.h"

typedef struct docker_network_ipam_config_t {
	char* subnet;
	char* gateway;
} docker_network_ipam_config;

d_err_t make_docker_network_ipam_config(docker_network_ipam_config** config, char* subnet, char* gateway);
void free_docker_network_ipam_config(docker_network_ipam_config* config);

typedef struct docker_network_ipam_t {
	char* driver;
	arraylist* config; //of docker_network_ipam_config
	arraylist* options;
} docker_network_ipam;

d_err_t make_docker_network_ipam(docker_network_ipam** ipam, char* driver);
void free_docker_network_ipam(docker_network_ipam* ipam);

typedef struct docker_network_container_t {
	char* id;
	char* name;
	char* endpoint_id;
	char* mac_address;
	char* ipv4_address;
	char* ipv6_address;
} docker_network_container;

d_err_t make_docker_network_container(docker_network_container** container,
		char* id, char* name, char* endpoint_id, char* mac_address,
		char* ipv4_address, char* ipv6_address);
void free_docker_network_container(docker_network_container* container);

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
	arraylist* containers; // of docker_network_container
	arraylist* options; //of pair
	arraylist* labels; //of pair
} docker_network;

d_err_t make_docker_network(docker_network** network, char* name,
		char* id, time_t created, char* scope, char* driver, int enableIPv6,
		docker_network_ipam* ipam, int internal, int attachable, int ingress);
void free_docker_network(docker_network* network);

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
d_err_t docker_networks_list(docker_context* ctx, docker_result** result,
		arraylist** networks, char* filter_driver, char* filter_id,
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
d_err_t docker_network_inspect(docker_context* ctx, docker_result** result,
		docker_network** net, char* id_or_name, int verbose, char* scope);

#ifdef __cplusplus 
}
#endif

#endif /* SRC_DOCKER_NETWORKS_H_ */
