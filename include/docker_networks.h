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

typedef json_object											docker_network_container;
#define docker_network_container_name_get(ctr)				get_attr_str((json_object*)ctr, "Name")
#define docker_network_container_endpoint_id_get(ctr)		get_attr_str((json_object*)ctr, "EndpointID")
#define docker_network_container_mac_address_get(ctr)		get_attr_str((json_object*)ctr, "MacAddress")
#define docker_network_container_ipv4_address_get(ctr)		get_attr_str((json_object*)ctr, "IPv4Address")
#define docker_network_container_ipv6_address_get(ctr)		get_attr_str((json_object*)ctr, "IPv6Address")

typedef json_object											docker_network_ipam;
#define docker_network_ipam_driver_get(ipam)				get_attr_str((json_object*)ipam, "Driver")
#define docker_network_ipam_config_length(ipam)				json_object_array_length(get_attr_json_object((json_object*)ipam, "Config"))
#define docker_network_ipam_config_get_idx(ipam, i)			json_object_array_get_idx(get_attr_json_object((json_object*)ipam, "Config"))

typedef json_object											docker_network;
#define free_docker_network(net)							json_object_put((json_object*) net)
#define docker_network_name_get(net)						get_attr_str((json_object*)net, "Name")
#define docker_network_id_get(net)							get_attr_str((json_object*)net, "Id")
#define docker_network_created_get(net)						get_attr_str((json_object*)net, "Created")
#define docker_network_scope_get(net)						get_attr_str((json_object*)net, "Scope")
#define docker_network_driver_get(net)						get_attr_str((json_object*)net, "Driver")
#define docker_network_enable_ipv6_get(net)					get_attr_boolean((json_object*)net, "EnableIPv6")
#define docker_network_internal_get(net)					get_attr_boolean((json_object*)net, "Internal")
#define docker_network_attachable_get(net)					get_attr_boolean((json_object*)net, "Attachable")
#define docker_network_ingress_get(net)						get_attr_boolean((json_object*)net, "Ingress")
#define docker_network_options_get(net)						get_attr_json_object((json_object*)net, "Options")
#define docker_network_options_foreach(net)					json_object_object_foreach(docker_network_options_get(net), key, val)
#define docker_network_labels_get(net)						get_attr_json_object((json_object*)net, "Labels")
#define docker_network_labels_foreach(net)					json_object_object_foreach(docker_network_labels_get(net), key, val)
#define docker_network_ipam_get(net)						(docker_network_ipam*)get_attr_json_object((json_object*)net, "IPAM")
#define docker_network_containers_get(net)					get_attr_json_object((json_object*)net, "Containers")
#define docker_network_containers_foreach(net)				json_object_object_foreach(docker_network_containers_get(net), key, val)

typedef json_object											docker_network_list;
#define free_docker_network_list(network_ls)				json_object_put(network_ls)
#define docker_network_list_length(network_ls)				json_object_array_length(network_ls)
#define docker_network_list_get_idx(network_ls, i)			(docker_network*) json_object_array_get_idx(network_ls, i)

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
		docker_network_list** networks, char* filter_driver, char* filter_id,
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
