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
#include "docker_util.h"

d_err_t make_docker_network_ipam_config(docker_network_ipam_config** config,
		char* subnet, char* gateway) {
	(*config) = (docker_network_ipam_config*) malloc(
			sizeof(docker_network_ipam_config*));
	if ((*config) == NULL) {
		return E_ALLOC_FAILED;
	}
	(*config)->gateway = make_defensive_copy(gateway);
	(*config)->subnet = make_defensive_copy(subnet);
	return E_SUCCESS;
}

void free_docker_network_ipam_config(docker_network_ipam_config* config) {
	free(config->gateway);
	free(config->subnet);
	free(config);
}

d_err_t make_docker_network_ipam(docker_network_ipam** ipam, char* driver) {
	(*ipam) = (docker_network_ipam*) malloc(sizeof(docker_network_ipam));
	if ((*ipam) == NULL) {
		return E_ALLOC_FAILED;
	}
	(*ipam)->driver = make_defensive_copy(driver);
	(*ipam)->config = array_list_new(
			(void (*)(void *)) &free_docker_network_ipam_config);
	(*ipam)->options = array_list_new((void (*)(void *)) &free_pair);
	return E_SUCCESS;
}

void free_docker_network_ipam(docker_network_ipam* ipam) {
	free(ipam->driver);
	array_list_free(ipam->config);
	array_list_free(ipam->options);
	free(ipam);
}

d_err_t make_docker_network_container(docker_network_container** container,
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

d_err_t make_docker_network(docker_network** network, char* name, char* id,
		time_t created, char* scope, char* driver, int enableIPv6,
		docker_network_ipam* ipam, int internal, int attachable, int ingress) {
	(*network) = (docker_network*) malloc(sizeof(docker_network));
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
void free_docker_network(docker_network* network) {

}

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
d_err_t docker_networks_list(docker_context* ctx, docker_result** result,
		struct array_list** networks, char* filter_driver, char* filter_id,
		char* filter_label, char* filter_name, char* filter_scope,
		char* filter_type) {
	char* url = create_service_url_id_method(NETWORK, NULL, "");

	struct array_list* params = array_list_new(
			(void (*)(void *)) &free_url_param);
	url_param* p;
	json_object* filters = make_filters();
	if (filter_driver != NULL) {
		add_filter_str(filters, "driver", make_defensive_copy(filter_driver));
	}
	if (filter_id != NULL) {
		add_filter_str(filters, "id", make_defensive_copy(filter_id));
	}
	if (filter_label != NULL) {
		add_filter_str(filters, "label", make_defensive_copy(filter_label));
	}
	if (filter_name != NULL) {
		add_filter_str(filters, "name", make_defensive_copy(filter_name));
	}
	if (filter_scope != NULL) {
		add_filter_str(filters, "scope", make_defensive_copy(filter_scope));
	}
	if (filter_type != NULL) {
		add_filter_str(filters, "type", make_defensive_copy(filter_type));
	}
	make_url_param(&p, "filters", (char *) filters_to_str(filters));
	array_list_add(params, p);

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_get(ctx, result, url, params, &chunk, &response_obj);

	(*networks) = array_list_new((void (*)(void *)) &free_docker_network);
	int num_nets = json_object_array_length(response_obj);
	for (int i = 0; i < num_nets; i++) {
		json_object* current_obj = json_object_array_get_idx(response_obj, i);
		docker_network* ni;

		docker_network_ipam* ipam;
		json_object* ipam_obj;
		json_object_object_get_ex(current_obj, "IPAM", &ipam_obj);
		make_docker_network_ipam(&ipam, get_attr_str(ipam_obj, "driver"));

		json_object* ipam_options_obj;
		json_object_object_get_ex(ipam_obj, "Options", &ipam_options_obj);
		if (ipam_options_obj != NULL) {
			json_object_object_foreach(ipam_options_obj, key, val)
			{
				pair* p;
				make_pair(&p, key, (char*) json_object_get_string(val));
				array_list_add(ipam->options, p);
			}
		}

		json_object* ipam_config_obj;
		json_object_object_get_ex(ipam_obj, "Config", &ipam_config_obj);
		if (ipam_config_obj != NULL) {
			int num_configs = json_object_array_length(ipam_config_obj);
			for (int j = 0; j < num_configs; j++) {
				json_object* current_config_obj = json_object_array_get_idx(
						ipam_config_obj, j);
				docker_network_ipam_config* config;
				make_docker_network_ipam_config(&config,
						get_attr_str(current_config_obj, "Subnet"),
						get_attr_str(current_config_obj, "Gateway"));
				array_list_add(ipam->config, config);
			}
		}

		make_docker_network(&ni, get_attr_str(current_obj, "Name"),
				get_attr_str(current_obj, "Id"),
				get_attr_unsigned_long(current_obj, "Created"),
				get_attr_str(current_obj, "Scope"),
				get_attr_str(current_obj, "Driver"),
				get_attr_int(current_obj, "EnableIPv6"), ipam,
				get_attr_int(current_obj, "Internal"),
				get_attr_int(current_obj, "Attachable"),
				get_attr_int(current_obj, "Ingress"));
		json_object* labels_obj;
		json_object_object_get_ex(current_obj, "Labels", &labels_obj);
		if (labels_obj != NULL) {
			json_object_object_foreach(labels_obj, key, val)
			{
				pair* p;
				make_pair(&p, key, (char*) json_object_get_string(val));
				array_list_add(ni->labels, p);
			}
		}
		json_object* options_obj;
		json_object_object_get_ex(current_obj, "Options", &options_obj);
		if (options_obj != NULL) {
			json_object_object_foreach(options_obj, key, val)
			{
				pair* p;
				make_pair(&p, key, (char*) json_object_get_string(val));
				array_list_add(ni->options, p);
			}
		}
		json_object* containers_obj;
		json_object_object_get_ex(current_obj, "Containers", &containers_obj);
		if (containers_obj != NULL) {
			json_object_object_foreach(containers_obj, key, val)
			{
				docker_network_container* container;
				make_docker_network_container(&container,
						get_attr_str(val, "Id"), get_attr_str(val, "Name"),
						get_attr_str(val, "EndpointID"),
						get_attr_str(val, "MacAddress"),
						get_attr_str(val, "IPv4Address"),
						get_attr_str(val, "IPv6Address"));
				array_list_add(ni->containers, container);
			}
		}
		array_list_add((*networks), ni);
	}

	array_list_free(params);
	return E_SUCCESS;
}

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
		docker_network** net, char* id_or_name, int verbose, char* scope) {
	if (id_or_name == NULL) {
		return E_INVALID_INPUT;
	}
	char* url = create_service_url_id_method(NETWORK, NULL, id_or_name);

	struct array_list* params = array_list_new(
			(void (*)(void *)) &free_url_param);
	url_param* p;
	if (verbose != 0) {
		make_url_param(&p, "verbose", make_defensive_copy("true"));
		array_list_add(params, p);
	}
	if (scope != NULL) {
		make_url_param(&p, "scope", make_defensive_copy(scope));
		array_list_add(params, p);
	}

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_get(ctx, result, url, params, &chunk, &response_obj);

	//If network was returned parse the response and return the details.
	if ((*result)->http_error_code == 200) {
		docker_network* ni;

		docker_network_ipam* ipam;
		json_object* ipam_obj;
		json_object_object_get_ex(response_obj, "IPAM", &ipam_obj);
		make_docker_network_ipam(&ipam, get_attr_str(ipam_obj, "driver"));

		json_object* ipam_options_obj;
		json_object_object_get_ex(ipam_obj, "Options", &ipam_options_obj);
		if (ipam_options_obj != NULL) {
			json_object_object_foreach(ipam_options_obj, key, val)
			{
				pair* p;
				make_pair(&p, key, (char*) json_object_get_string(val));
				array_list_add(ipam->options, p);
			}
		}

		json_object* ipam_config_obj;
		json_object_object_get_ex(ipam_obj, "Config", &ipam_config_obj);
		int num_configs = json_object_array_length(ipam_config_obj);
		if (ipam_config_obj != NULL) {
			int num_configs = json_object_array_length(ipam_config_obj);
			for (int j = 0; j < num_configs; j++) {

				json_object* current_config_obj = json_object_array_get_idx(
						ipam_config_obj, j);
				docker_network_ipam_config* config;
				make_docker_network_ipam_config(&config,
						get_attr_str(current_config_obj, "Subnet"),
						get_attr_str(current_config_obj, "Gateway"));
				array_list_add(ipam->config, config);
			}
		}

		make_docker_network(&ni, get_attr_str(response_obj, "Name"),
				get_attr_str(response_obj, "Id"),
				get_attr_unsigned_long(response_obj, "Created"),
				get_attr_str(response_obj, "Scope"),
				get_attr_str(response_obj, "Driver"),
				get_attr_int(response_obj, "EnableIPv6"), ipam,
				get_attr_int(response_obj, "Internal"),
				get_attr_int(response_obj, "Attachable"),
				get_attr_int(response_obj, "Ingress"));
		json_object* labels_obj;
		json_object_object_get_ex(response_obj, "Labels", &labels_obj);
		if (labels_obj != NULL) {
			json_object_object_foreach(labels_obj, key, val)
			{
				pair* p;
				make_pair(&p, key, (char*) json_object_get_string(val));
				array_list_add(ni->labels, p);
			}
		}
		json_object* options_obj;
		json_object_object_get_ex(response_obj, "Options", &options_obj);
		if (options_obj != NULL) {
			json_object_object_foreach(options_obj, key, val)
			{
				pair* p;
				make_pair(&p, key, (char*) json_object_get_string(val));
				array_list_add(ni->options, p);
			}
		}
		json_object* containers_obj;
		json_object_object_get_ex(response_obj, "Containers", &containers_obj);
		if (containers_obj != NULL) {
			json_object_object_foreach(containers_obj, key, val)
			{
				docker_network_container* container;
				make_docker_network_container(&container,
						get_attr_str(val, "Id"), get_attr_str(val, "Name"),
						get_attr_str(val, "EndpointID"),
						get_attr_str(val, "MacAddress"),
						get_attr_str(val, "IPv4Address"),
						get_attr_str(val, "IPv6Address"));
				array_list_add(ni->containers, container);
			}
		}

		(*net) = ni;
	}

	free(params);
	return E_SUCCESS;
}
