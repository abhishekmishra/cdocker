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
	(*ipam) = (docker_network_ipam*) malloc(sizeof(docker_network_ipam));
	if ((*ipam) == NULL) {
		return E_ALLOC_FAILED;
	}
	(*ipam)->driver = make_defensive_copy(driver);
	(*ipam)->config = array_list_new((void (*)(void *)) &free_pair);
	(*ipam)->options = array_list_new((void (*)(void *)) &free_pair);
	return E_SUCCESS;
}

void free_docker_network_ipam(docker_network_ipam* ipam) {
	free(ipam->driver);
	array_list_free(ipam->config);
	array_list_free(ipam->options);
	free(ipam);
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
	(*container) = (docker_network_container*) malloc(
			sizeof(docker_network_container));
	if ((*container) == NULL) {
		return E_ALLOC_FAILED;
	}
	(*container)->id = make_defensive_copy(id);
	(*container)->name = make_defensive_copy(name);
	(*container)->endpoint_id = make_defensive_copy(endpoint_id);
	(*container)->mac_address = make_defensive_copy(mac_address);
	(*container)->ipv4_address = make_defensive_copy(ipv4_address);
	(*container)->ipv6_address = make_defensive_copy(ipv6_address);
	return E_SUCCESS;
}

void free_docker_network_container(docker_network_container* container) {
	free(container->id);
	free(container->name);
	free(container->endpoint_id);
	free(container->mac_address);
	free(container->ipv4_address);
	free(container->ipv6_address);
	free(container);
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
	(*network) = (docker_network_item*) malloc(sizeof(docker_network_item));
	if ((*network) == NULL) {
		return E_ALLOC_FAILED;
	}
	(*network)->name = make_defensive_copy(name);
	(*network)->id = make_defensive_copy(id);
	(*network)->created = created;
	(*network)->scope = make_defensive_copy(scope);
	(*network)->driver = make_defensive_copy(driver);
	(*network)->enableIPv6 = enableIPv6;
	(*network)->ipam = ipam;
	(*network)->internal = internal;
	(*network)->attachable = attachable;
	(*network)->ingress = ingress;
	(*network)->containers = array_list_new(
			(void (*)(void *)) &free_docker_network_container);
	(*network)->options = array_list_new((void (*)(void *)) &free_pair);
	(*network)->labels = array_list_new((void (*)(void *)) &free_pair);
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
DOCKER_NETWORK_GETTER_ARR_ADD_IMPL(item, docker_network_container*, containers)
DOCKER_NETWORK_GETTER_ARR_LEN_IMPL(item, containers)
DOCKER_NETWORK_GETTER_ARR_GET_IDX_IMPL(item, docker_network_container*,
		containers)
DOCKER_NETWORK_GETTER_ARR_ADD_IMPL(item, pair*, options)
DOCKER_NETWORK_GETTER_ARR_LEN_IMPL(item, options)
DOCKER_NETWORK_GETTER_ARR_GET_IDX_IMPL(item, pair*, options)
DOCKER_NETWORK_GETTER_ARR_ADD_IMPL(item, pair*, labels)
DOCKER_NETWORK_GETTER_ARR_LEN_IMPL(item, labels)
DOCKER_NETWORK_GETTER_ARR_GET_IDX_IMPL(item, pair*, labels)

/**
 * List all networks which match the filters given.
 * If all filters are null, then all networks are listed.
 *
 * \param ctx docker context
 * \param result the result object to be returned
 * \param networks the arraylist of networks to be returned
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
		char* filter_type) {
	char* url = create_service_url_id_method(NETWORK, NULL, "");

	struct array_list* params = array_list_new(
			(void (*)(void *)) &free_url_param);
	url_param* p;
	if (filter_driver != NULL) {
		make_url_param(&p, "driver", make_defensive_copy(filter_driver));
		array_list_add(params, p);
	}
	if (filter_id != NULL) {
		make_url_param(&p, "id", make_defensive_copy(filter_id));
		array_list_add(params, p);
	}
	if (filter_label != NULL) {
		make_url_param(&p, "label", make_defensive_copy(filter_label));
		array_list_add(params, p);
	}
	if (filter_name != NULL) {
		make_url_param(&p, "name", make_defensive_copy(filter_name));
		array_list_add(params, p);
	}
	if (filter_scope != NULL) {
		make_url_param(&p, "scope", make_defensive_copy(filter_scope));
		array_list_add(params, p);
	}
	if (filter_type != NULL) {
		make_url_param(&p, "type", make_defensive_copy(filter_type));
		array_list_add(params, p);
	}

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_get(ctx, result, url, params, &chunk, &response_obj);

	(*networks) = array_list_new((void (*)(void *)) &free_docker_network_item);
	int num_nets = json_object_array_length(response_obj);
	for (int i = 0; i < num_nets; i++) {
		json_object* current_obj = json_object_array_get_idx(response_obj, i);
		docker_network_ipam* ipam;
		json_object* ipam_obj;
		json_object_object_get_ex(current_obj, "IPAM", &ipam_obj);
		make_docker_network_ipam(&ipam, get_attr_str(ipam_obj, "driver"));
		docker_network_item* ni;
		make_docker_network_item(&ni, get_attr_str(current_obj, "Name"),
				get_attr_str(current_obj, "Id"),
				get_attr_unsigned_long(current_obj, "Created"),
				get_attr_str(current_obj, "Scope"),
				get_attr_str(current_obj, "Driver"),
				get_attr_int(current_obj, "EnableIPv6"), ipam,
				get_attr_int(current_obj, "Internal"),
				get_attr_int(current_obj, "Attachable"),
				get_attr_int(current_obj, "Ingress"));
		array_list_add((*networks), ni);
	}

	array_list_free(params);
	return E_SUCCESS;
}

