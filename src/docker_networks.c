/**
 * Copyright (c) 2020 Abhishek Mishra
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "docker_util.h"
#include "docker_networks.h"

d_err_t docker_networks_list(docker_context* ctx, 
		docker_network_list** networks, char* filter_driver, char* filter_id,
		char* filter_label, char* filter_name, char* filter_scope,
		char* filter_type) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, NETWORK, NULL, "") != 0) {
		return E_ALLOC_FAILED;
	}

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
	char* filter_str = (char*)filters_to_str(filters);
	docker_call_params_add(call, "filters", filter_str);
	free(filter_str);

	d_err_t err = docker_call_exec(ctx, call, networks);

	free_docker_call(call);
	return err;
}

d_err_t docker_network_inspect(docker_context* ctx, 
		docker_network** net, char* id_or_name, int verbose, char* scope) {
	if (id_or_name == NULL) {
		return E_INVALID_INPUT;
	}
	docker_call* call;
	if (make_docker_call(&call, ctx->url, NETWORK, NULL, id_or_name) != 0) {
		return E_ALLOC_FAILED;
	}

	if (verbose != 0) {
		docker_call_params_add(call, "verbose", str_clone("true"));
	}
	if (scope != NULL) {
		docker_call_params_add(call, "scope", str_clone(scope));
	}

	d_err_t err = docker_call_exec(ctx, call, net);

	free_docker_call(call);
	return err;
}
