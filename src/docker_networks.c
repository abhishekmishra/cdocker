/*
 * clibdocker: docker_networks.c
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

#include <json-c/arraylist.h>
#include "docker_networks.h"

#define DOCKER_NETWORK_GETTER_IMPL(object, type, name) \
	type docker_network_ ## object ## _get_ ## name(docker_network_ ## object* object) { \
		return object->name; \
	} \


#define DOCKER_NETWORK_GETTER_ARR_ADD_IMPL(object, type, name) \
	int docker_network_ ## object ## _ ## name ## _add(docker_network_ ## object* object, type data) { \
		return array_list_add(object->name, (void*) data); \
	} \

#define DOCKER_NETWORK_GETTER_ARR_LEN_IMPL(object, name) \
	int docker_network_ ## object ## _ ## name ##_length(docker_network_ ## object* object) { \
		return array_list_length(object->name); \
	} \

#define DOCKER_NETWORK_GETTER_ARR_GET_IDX_IMPL(object, type, name) \
	type docker_network_ ## object ## _ ## name ## _get_idx(docker_network_ ## object* object, int i) { \
		return (type) array_list_get_idx(object->name, i); \
	} \


error_t make_docker_network_ipam(docker_network_ipam** ipam, char* driver) {
	//TODO
	return E_SUCCESS;
}
void free_docker_network_ipam(docker_network_ipam* ipam) {
	//TODO
}
DOCKER_NETWORK_GETTER_IMPL(ipam, char*, driver)
DOCKER_NETWORK_GETTER_ARR_ADD_IMPL(ipam, pair*, config)
DOCKER_NETWORK_GETTER_ARR_LEN_IMPL(ipam, config)
DOCKER_NETWORK_GETTER_ARR_GET_IDX_IMPL(ipam, pair*, config)
DOCKER_NETWORK_GETTER_ARR_ADD_IMPL(ipam, pair*, options)
DOCKER_NETWORK_GETTER_ARR_LEN_IMPL(ipam, options)
DOCKER_NETWORK_GETTER_ARR_GET_IDX_IMPL(ipam, pair*, options)

error_t make_docker_network_container(docker_network_container** container,
		char* id, char* name, char* endpoint_id, char* mac_address,
		char* ipv4_address, char* ipv6_address) {
	//TODO
	return E_SUCCESS;
}
void free_docker_network_container(docker_network_container* container) {
	//TODO
}
DOCKER_NETWORK_GETTER_IMPL(container, char*, id)
DOCKER_NETWORK_GETTER_IMPL(container, char*, name)
DOCKER_NETWORK_GETTER_IMPL(container, char*, endpoint_id)
DOCKER_NETWORK_GETTER_IMPL(container, char*, mac_address)
DOCKER_NETWORK_GETTER_IMPL(container, char*, ipv4_address)
DOCKER_NETWORK_GETTER_IMPL(container, char*, ipv6_address)

error_t make_docker_network_item(docker_network_item** network, char* name,
		char* id, time_t created, char* scope, char* driver, int enableIPv6,
		docker_network_ipam* ipam, int internal, int attachable, int ingress) {
	//TODO
	return E_SUCCESS;
}
void free_docker_network_item(docker_network_item* network) {

}
DOCKER_NETWORK_GETTER_IMPL(item, char*, name)
DOCKER_NETWORK_GETTER_IMPL(item, char*, id)
DOCKER_NETWORK_GETTER_IMPL(item, time_t, created)
DOCKER_NETWORK_GETTER_IMPL(item, char*, scope)
DOCKER_NETWORK_GETTER_IMPL(item, char*, driver)
DOCKER_NETWORK_GETTER_IMPL(item, int, enableIPv6)
DOCKER_NETWORK_GETTER_IMPL(item, docker_network_ipam*, ipam)
DOCKER_NETWORK_GETTER_IMPL(item, int, internal)
DOCKER_NETWORK_GETTER_IMPL(item, int, attachable)
DOCKER_NETWORK_GETTER_IMPL(item, int, ingress)

/**
 * List all networks which match the filters given.
 * If all filters are null, then all networks are listed.
 *
 * \param ctx docker context
 * \param result the result object to be returned
 * \param containers the arraylist of containers to be returned
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
		char* filter_type) {
	//TODO implementation
	return E_SUCCESS;
}

