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

/**
 * @brief Docker Network Container json object
 * 
 * This object represents the container details inside the response of
 * the docker network api call response.
 */
typedef json_object											docker_network_container;

/**
 * @brief Get the name of the container from the docker container
 * 
 * @param ctr docker container
 * @return char* name
 */
#define docker_network_container_name_get(ctr)				get_attr_str((json_object*)ctr, "Name")

/**
 * @brief Get the endpoint id of the container from the docker container
 * 
 * @param ctr docker container
 * @return char* endpoint id
 */
#define docker_network_container_endpoint_id_get(ctr)		get_attr_str((json_object*)ctr, "EndpointID")

/**
 * @brief Get the mac address of the container from the docker container
 * 
 * @param ctr docker container
 * @return char* mac address
 */
#define docker_network_container_mac_address_get(ctr)		get_attr_str((json_object*)ctr, "MacAddress")

/**
 * @brief Get the ipv4 of the container from the docker container
 * 
 * @param ctr docker container
 * @return char* ipv4
 */
#define docker_network_container_ipv4_address_get(ctr)		get_attr_str((json_object*)ctr, "IPv4Address")

/**
 * @brief Get the ipv6 of the container from the docker container
 * 
 * @param ctr docker container
 * @return char* ipv6
 */
#define docker_network_container_ipv6_address_get(ctr)		get_attr_str((json_object*)ctr, "IPv6Address")

/**
 * @brief Docker Network Ipam json object
 */
typedef json_object											docker_network_ipam;

/**
 * @brief Get the ipam driver
 *
 * @param ipam network ipam object
 * @return char* driver
 */
#define docker_network_ipam_driver_get(ipam)				get_attr_str((json_object*)ipam, "Driver")

/**
 * @brief Get the ipam config length
 *
 * @param ipam network ipam object
 * @return size_t ipam config length
 */
#define docker_network_ipam_config_length(ipam)				json_object_array_length(get_attr_json_object((json_object*)ipam, "Config"))

/**
 * @brief Get ith ipam config
 *
 * @param ipam network ipam object
 * @return json_object* ith ipam config object
 */
#define docker_network_ipam_config_get_idx(ipam, i)			json_object_array_get_idx(get_attr_json_object((json_object*)ipam, "Config"))

/**
 * @brief Docker Network json object
 */
typedef json_object											docker_network;

/**
 * @brief Free the docker network object
 * 
 * @param net docker network object
 */
#define free_docker_network(net)							json_object_put((json_object*) net)

/**
 * @brief Get the name of the docker network
 * 
 * @param net docker network object
 * @return char* name
 */
#define docker_network_name_get(net)						get_attr_str((json_object*)net, "Name")

/**
 * @brief Get the id of the docker network
 * 
 * @param net docker network object
 * @return char* id
 */
#define docker_network_id_get(net)							get_attr_str((json_object*)net, "Id")

/**
 * @brief Get the created datetime of the docker network
 * 
 * @param net docker network object
 * @return char* created datetime
 */
#define docker_network_created_get(net)						get_attr_str((json_object*)net, "Created")

/**
 * @brief Get the scope of the docker network
 * 
 * @param net docker network object
 * @return char* scope
 */
#define docker_network_scope_get(net)						get_attr_str((json_object*)net, "Scope")

/**
 * @brief Get the network driver of the docker network
 * 
 * @param net docker network object
 * @return char* network driver
 */
#define docker_network_driver_get(net)						get_attr_str((json_object*)net, "Driver")

/**
 * @brief Get the EnableIPv6 flag of the docker network
 * 
 * @param net docker network object
 * @return bool enable ipv6 flag
 */
#define docker_network_enable_ipv6_get(net)					get_attr_boolean((json_object*)net, "EnableIPv6")

/**
 * @brief Get the Internal flag of the docker network
 * 
 * @param net docker network object
 * @return bool internal flag
 */
#define docker_network_internal_get(net)					get_attr_boolean((json_object*)net, "Internal")

/**
 * @brief Get the Attachable flag of the docker network
 * 
 * @param net docker network object
 * @return bool attachable flag
 */
#define docker_network_attachable_get(net)					get_attr_boolean((json_object*)net, "Attachable")

/**
 * @brief Get the Ingress flag of the docker network
 * 
 * @param net docker network object
 * @return bool ingress flag
 */
#define docker_network_ingress_get(net)						get_attr_boolean((json_object*)net, "Ingress")

/**
 * @brief Get the network options json object of the docker network
 * 
 * @param net docker network object
 * @return json_object* network options
 */
#define docker_network_options_get(net)						get_attr_json_object((json_object*)net, "Options")

/**
 * @brief Iterate the network options of the docker network
 * 
 * @param net docker network object
 */
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
 * \param networks the array_list of networks to be returned
 * \param filter_driver
 * \param filter_id
 * \param filter_label
 * \param filter_name
 * \param filter_scope
 * \param filter_type
 * \return error code
 */
MODULE_API d_err_t docker_networks_list(docker_context* ctx, docker_network_list** networks, 
		char* filter_driver, char* filter_id,
		char* filter_label, char* filter_name, char* filter_scope,
		char* filter_type);

/**
 * Inspect details of a network looked up by name or id.
 *
 * \param ctx docker context
 * \param net details of the network returned
 * \param id_or_name id or name of the network to be looked up
 * \param verbose whether inspect output is verbose (0 means false, true otherwise)
 * \param scope filter by one of swarm, global, or local
 * \return error code
 */
MODULE_API d_err_t docker_network_inspect(docker_context* ctx, docker_network** net, 
	char* id_or_name, int verbose, char* scope);

#ifdef __cplusplus 
}
#endif

#endif /* SRC_DOCKER_NETWORKS_H_ */
