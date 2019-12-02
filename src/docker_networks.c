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

#include "docker_util.h"
#include "docker_networks.h"

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
		docker_network_list** networks, char* filter_driver, char* filter_id,
		char* filter_label, char* filter_name, char* filter_scope,
		char* filter_type) {
	char* url = create_service_url_id_method(NETWORK, NULL, "");
	if (url == NULL) {
		return E_ALLOC_FAILED;
	}

	arraylist* params;
	arraylist_new(&params, 
			(void (*)(void *)) &free_url_param);
	url_param* p;
	json_object* filters = make_filters();
	if (filter_driver != NULL) {
		add_filter_str(filters, "driver", str_clone(filter_driver));
	}
	if (filter_id != NULL) {
		add_filter_str(filters, "id", str_clone(filter_id));
	}
	if (filter_label != NULL) {
		add_filter_str(filters, "label", str_clone(filter_label));
	}
	if (filter_name != NULL) {
		add_filter_str(filters, "name", str_clone(filter_name));
	}
	if (filter_scope != NULL) {
		add_filter_str(filters, "scope", str_clone(filter_scope));
	}
	if (filter_type != NULL) {
		add_filter_str(filters, "type", str_clone(filter_type));
	}
	make_url_param(&p, "filters", (char *) filters_to_str(filters));
	arraylist_add(params, p);

	struct http_response_memory chunk;
	docker_api_get(ctx, result, url, params, &chunk, networks);

	if (chunk.memory != NULL) {
		free(chunk.memory);
	}
	arraylist_free(params);
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

	arraylist* params;
	arraylist_new(&params, 
			(void (*)(void *)) &free_url_param);
	url_param* p;
	if (verbose != 0) {
		make_url_param(&p, "verbose", str_clone("true"));
		arraylist_add(params, p);
	}
	if (scope != NULL) {
		make_url_param(&p, "scope", str_clone(scope));
		arraylist_add(params, p);
	}

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_get(ctx, result, url, params, &chunk, net);

	arraylist_free(params);
	if (chunk.memory != NULL) {
		free(chunk.memory);
	}
	return E_SUCCESS;
}
