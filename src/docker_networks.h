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

#include <json-c/arraylist.h>

typedef struct ipam_config_t {
	char* key;
	char* value;
} ipam_config;

typedef struct ipam_t {
	char* driver;
	struct arraylist* config; //of ipam_config
	struct arraylist* options;
} ipam;

typedef struct docker_network_container_t {
	char* id;
	char* name;
	char* endpoint_id;
	char* mac_address;
	char* ipv4_address;
	char* ipv6_address;
} docker_network_container;

typedef struct docker_network_t {
	char* name;
	char* id;
	time_t created;
	char* scope;
	char* driver;
	int enableIPv6;
	ipam* ipam;
	int internal;
	int attachable;
	int ingress;
	struct arraylist* containers; // of docker_network_container
	struct arraylist* options; //of pair
	struct arraylist* labels; //of pair

} docker_network;

error_t docker_networks_list(docker_context* ctx, docker_result** result,
		struct arraylist** containers, char* filter_driver, char* filter_id,
		char* filter_label, char* filter_name, char* filter_scope,
		char* filter_type);

#endif /* SRC_DOCKER_NETWORKS_H_ */
