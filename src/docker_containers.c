/*
 * docker_containers.c
 *
 *  Created on: 11-Dec-2018
 *      Author: abhishek
 */

#include <docker_log.h>
#include "docker_containers.h"

#include <json-c/arraylist.h>
#include <json-c/json_object.h>
#include <json-c/json_tokener.h>
#include <json-c/linkhash.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <strings.h>
#include "docker_connection_util.h"
#include "docker_util.h"

/**
 * Create a docker_container_ports instance.
 *
 * \param ports the instance to create.
 * \param priv the private port
 * \param pub the public port
 * \param type port type
 * \return error code
 */
d_err_t make_docker_container_ports(docker_container_ports** ports, long priv,
		long pub, char* type) {
	(*ports) = (docker_container_ports*) malloc(sizeof(docker_container_ports));
	if (!(*ports)) {
		return E_ALLOC_FAILED;
	}
	(*ports)->private_port = priv;
	(*ports)->public_port = pub;
	if (strcmp(type, "tcp") == 0) {
		(*ports)->type = CONTAINER_PORT_TYPE_TCP;
	}
	return E_SUCCESS;
}

void free_docker_container_ports(docker_container_ports* ports) {
	free(ports);
}

/**
 * Create a new label.
 *
 * \param label ptr to return value.
 * \param key
 * \param value
 * \return error code
 */
d_err_t make_docker_container_label(docker_container_label** label,
		const char* key, const char* value) {
	(*label) = (docker_container_label*) malloc(sizeof(docker_container_label));
	if (!(*label)) {
		return E_ALLOC_FAILED;
	}
	(*label)->key = make_defensive_copy(key);
	(*label)->value = make_defensive_copy(value);
	if ((*label)->key && (*label)->value) {
		return E_SUCCESS;
	} else {
		return E_ALLOC_FAILED;
	}
}

void free_docker_container_label(docker_container_label* label) {
	free(label->key);
	free(label->value);
	free(label);
}

/**
 * Create a new host config
 *
 * \param pointer to return instance
 * \param network_mode
 * \return error_code
 */
d_err_t make_docker_container_host_config(
		docker_container_host_config** host_config, const char* network_mode) {
	(*host_config) = (docker_container_host_config*) malloc(
			sizeof(docker_container_host_config));
	if (!(*host_config)) {
		return E_ALLOC_FAILED;
	}
	(*host_config)->network_mode = make_defensive_copy(network_mode);
	return E_SUCCESS;
}

void free_docker_container_host_config(
		docker_container_host_config* host_config) {
	free(host_config->network_mode);
	free(host_config);
}

/**
 * Create a new network settings item
 */
d_err_t make_docker_container_network_settings_item(
		docker_container_network_settings_item** item, const char* name,
		const char* network_id, const char* endpoint_id, const char* gateway,
		const char* ip_address, int ip_prefix_len, const char* ipv6_gateway,
		const char* global_ipv6_address, int global_ipv6_prefix_len,
		const char* mac_address) {
	(*item) = (docker_container_network_settings_item*) malloc(
			sizeof(docker_container_network_settings_item));
	docker_container_network_settings_item* si = (*item);
	si->name = make_defensive_copy(name);
	si->network_id = make_defensive_copy(network_id);
	si->endpoint_id = make_defensive_copy(endpoint_id);
	si->gateway = make_defensive_copy(gateway);
	si->ip_address = make_defensive_copy(ip_address);
	si->ip_prefix_len = ip_prefix_len;
	si->ipv6_gateway = make_defensive_copy(ipv6_gateway);
	si->global_ipv6_address = make_defensive_copy(global_ipv6_address);
	si->global_ipv6_prefix_len = global_ipv6_prefix_len;
	si->mac_address = make_defensive_copy(mac_address);
	return E_SUCCESS;
}

void free_docker_container_network_settings_item(
		docker_container_network_settings_item* settings_item) {
	free(settings_item->name);
	free(settings_item->network_id);
	free(settings_item->endpoint_id);
	free(settings_item->gateway);
	free(settings_item->ip_address);
	free(settings_item->ipv6_gateway);
	free(settings_item->global_ipv6_address);
	free(settings_item->mac_address);
	free(settings_item);
}

/**
 * Create a new mount object
 */
d_err_t make_docker_container_mount(docker_container_mount** mount,
		const char* name, const char* type, const char* source,
		const char* destination, const char* driver, const char* mode,
		const int rw, const char* propagation) {
	(*mount) = (docker_container_mount*) malloc(sizeof(docker_container_mount));
	docker_container_mount* m = (*mount);
	m->name = make_defensive_copy(name);
	m->type = make_defensive_copy(type);
	m->source = make_defensive_copy(source);
	m->destination = make_defensive_copy(destination);
	m->driver = make_defensive_copy(driver);
	m->mode = make_defensive_copy(mode);
	m->rw = rw;
	m->propagation = make_defensive_copy(propagation);
	return E_SUCCESS;
}

void free_docker_container_mount(docker_container_mount* mount) {
	free(mount->name);
	free(mount->type);
	free(mount->source);
	free(mount->destination);
	free(mount->driver);
	free(mount->mode);
	free(mount->propagation);
	free(mount);
}

/**
 * Create a new containers list item.
 */
d_err_t make_docker_containers_list_item(docker_container_list_item** item,
		const char* id, const char* image, const char* image_id,
		const char* command, const long long created, const char* state,
		const char* status, const long long size_rw,
		const long long size_root_fs,
		const docker_container_host_config* hostConfig) {
	(*item) = (docker_container_list_item*) malloc(
			sizeof(docker_container_list_item));
	docker_container_list_item* li = (*item);

	li->id = make_defensive_copy(id);
	li->image = make_defensive_copy(image);
	li->image_id = make_defensive_copy(image_id);
	li->command = make_defensive_copy(command);
	li->created = created;
	li->state = make_defensive_copy(state);
	li->status = make_defensive_copy(status);
	li->size_rw = size_rw;
	li->size_root_fs = size_root_fs;

	li->names = array_list_new(&free);
	li->ports = array_list_new((void (*)(void *)) &free_docker_container_ports);
	li->labels = array_list_new(
			(void (*)(void *)) &free_docker_container_label);
	li->network_settings = array_list_new(
			(void (*)(void *)) &free_docker_container_network_settings_item);
	li->mounts = array_list_new(
			(void (*)(void *)) &free_docker_container_mount);
	return E_SUCCESS;
}

void free_docker_container_list_item(docker_container_list_item* item) {
	free(item->id);
	free(item->image);
	free(item->image_id);
	free(item->command);
	free(item->state);
	free(item->status);

	array_list_free(item->names);
	array_list_free(item->ports);
	array_list_free(item->labels);
	array_list_free(item->network_settings);
	array_list_free(item->mounts);
	free(item);
}

void extract_filter_field_char(json_object* fobj, char* filter_name,
		int num_items, char** items) {
	if (num_items > 0) {
		json_object* arr = json_object_new_array();
		for (int j = 0; j < num_items; j++) {
			json_object_array_add(arr, json_object_new_string(items[j]));
		}
		json_object_object_add(fobj, filter_name, arr);
	}
}

void extract_filter_field_int(json_object* fobj, char* filter_name,
		int num_items, int* items) {
	if (num_items > 0) {
		json_object* arr = json_object_new_array();
		for (int j = 0; j < num_items; j++) {
			json_object_array_add(arr, json_object_new_int64(items[j]));
		}
		json_object_object_add(fobj, filter_name, arr);
	}
}

d_err_t make_docker_containers_list(docker_containers_list** container_list) {
	(*container_list) = array_list_new(
			(void (*)(void *)) &free_docker_container_list_item);
	return E_SUCCESS;
}

int docker_containers_list_add(docker_containers_list* list,
		docker_container_list_item* item) {
	return array_list_add(list, item);
}

docker_container_list_item* docker_containers_list_get_idx(
		docker_containers_list* list, int i) {
	return (docker_container_list_item*) array_list_get_idx(list, i);
}

int docker_containers_list_length(docker_containers_list* list) {
	return array_list_length(list);
}

/**
 * List docker containers
 *
 * \param ctx the docker context
 * \param result the result object to return
 * \param container_list array_list of containers to be returned
 * \param all all or running only
 * \param limit max containers to return
 * \param size return the size of containers in response
 * \param varargs pairs of filters char* filter_name, char* filter_value (terminated by a NULL)
 * \return error code
 */
d_err_t docker_container_list(docker_context* ctx, docker_result** result,
		docker_containers_list** container_list, int all, int limit, int size,
		...) {
	char* method = "json";
	char* containers = "containers/";
	char* url = (char*) malloc(
			(strlen(containers) + strlen(method) + 1) * sizeof(char));
	sprintf(url, "%s%s", containers, method);
	docker_log_debug("List url is %s\n", url);

	struct array_list* params = array_list_new(
			(void (*)(void *)) &free_url_param);
	url_param* p;

	if (all > 0) {
		make_url_param(&p, "all", "true");
		array_list_add(params, p);
	}

	if (limit > 0) {
		char* lim_val = (char*) malloc(128 * sizeof(char));
		sprintf(lim_val, "%d", limit);
		make_url_param(&p, "limit", lim_val);
		array_list_add(params, p);
	}

	if (size > 0) {
		make_url_param(&p, "size", "true");
		array_list_add(params, p);
	}

	va_list kvargs;
	va_start(kvargs, size);
	json_object* filters = make_filters();
	while (true) {
		char* filter_name = va_arg(kvargs, char*);
		if (filter_name == NULL) {
			break;
		}
		char* filter_value = va_arg(kvargs, char*);
		if (filter_value == NULL) {
			break;
		}
		add_filter_str(filters, filter_name, filter_value);
	}
	make_url_param(&p, "filters", (char*) filters_to_str(filters));
	array_list_add(params, p);

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_get(ctx, result, url, params, &chunk, &response_obj);
	free(params);

	struct array_list* containers_arr = json_object_get_array(response_obj);
	int len = containers_arr->length;

	(*container_list) = array_list_new(
			(void (*)(void *)) &free_docker_container_list_item);
	struct array_list* clist = (*container_list);

	for (int i = 0; i < containers_arr->length; i++) {
		docker_log_debug("Item #%d is %s\n", i,
				json_object_to_json_string(containers_arr->array[i]));
		docker_container_list_item* listItem;

		json_object* hostConfigObj;
		docker_container_host_config* host_config;
		if (json_object_object_get_ex(containers_arr->array[i], "HostConfig",
				&hostConfigObj)) {
			make_docker_container_host_config(&host_config,
					get_attr_str(hostConfigObj, "NetworkMode"));
		}
		free(hostConfigObj);

		make_docker_containers_list_item(&listItem,
				get_attr_str(containers_arr->array[i], "Id"),
				get_attr_str(containers_arr->array[i], "Image"),
				get_attr_str(containers_arr->array[i], "ImageID"),
				get_attr_str(containers_arr->array[i], "Command"),
				get_attr_long_long(containers_arr->array[i], "Created"),
				get_attr_str(containers_arr->array[i], "State"),
				get_attr_str(containers_arr->array[i], "Status"),
				get_attr_long_long(containers_arr->array[i], "SizeRW"),
				get_attr_long_long(containers_arr->array[i], "SizeRootFs"),
				host_config);

		json_object* namesObj;
		if (json_object_object_get_ex(containers_arr->array[i], "Names",
				&namesObj)) {
			struct array_list* names_arr = json_object_get_array(namesObj);
			for (int ni = 0; ni < names_arr->length; ni++) {
				array_list_add(listItem->names,
						(char*) json_object_get_string(names_arr->array[ni]));
			}
		}
		free(namesObj);

		json_object* portsObj;
		if (json_object_object_get_ex(containers_arr->array[i], "Ports",
				&portsObj)) {
			struct array_list* ports_arr = json_object_get_array(portsObj);
			for (int ni = 0; ni < ports_arr->length; ni++) {
				docker_container_ports* ports;
				make_docker_container_ports(&ports,
						get_attr_long(ports_arr->array[ni], "PrivatePort"),
						get_attr_long(ports_arr->array[ni], "PublicPort"),
						get_attr_str(ports_arr->array[ni], "Type"));
				array_list_add(listItem->ports, ports);
			}
		}
		free(portsObj);

		json_object* labelsObj;
		if (json_object_object_get_ex(containers_arr->array[i], "Labels",
				&labelsObj)) {
			json_object_object_foreach(labelsObj, key1, val1)
			{
				docker_container_label* label;
				make_docker_container_label(&label, key1,
						json_object_get_string((json_object *) val1));
				docker_log_debug("Label [%s] = [%s]", label->key, label->value);
				array_list_add(listItem->labels, label);
			}
		}
		free(labelsObj);

		json_object* networkSettingsObj;
		if (json_object_object_get_ex(containers_arr->array[i],
				"NetworkSettings", &networkSettingsObj)) {
			json_object* networksObj;
			if (json_object_object_get_ex(networkSettingsObj, "Networks",
					&networksObj)) {

				json_object_object_foreach(networksObj, k, v) {
					docker_container_network_settings_item* settings;
					make_docker_container_network_settings_item(&settings, k,
							get_attr_str(v, "NetworkID"),
							get_attr_str(v, "EndpointID"),
							get_attr_str(v, "Gateway"),
							get_attr_str(v, "IPAddress"),
							get_attr_int(v, "IPPrefixLen"),
							get_attr_str(v, "IPv6Gateway"),
							get_attr_str(v, "GlobalIPv6Address"),
							get_attr_int(v, "GlobalIPv6PrefixLen"),
							get_attr_str(v, "MacAddress"));
					array_list_add(listItem->network_settings, settings);
				}
				free(networksObj);
			}
			free(networkSettingsObj);
		}

		json_object* mountsObj;
		if (json_object_object_get_ex(containers_arr->array[i], "Mounts",
				&mountsObj)) {
			struct array_list* mounts_arr = json_object_get_array(mountsObj);
			for (int ni = 0; ni < mounts_arr->length; ni++) {
				docker_container_mount* mount;
				make_docker_container_mount(&mount,
						get_attr_str(mounts_arr->array[ni], "Name"),
						get_attr_str(mounts_arr->array[ni], "Type"),
						get_attr_str(mounts_arr->array[ni], "Source"),
						get_attr_str(mounts_arr->array[ni], "Destination"),
						get_attr_str(mounts_arr->array[ni], "Driver"),
						get_attr_str(mounts_arr->array[ni], "Mode"),
						get_attr_int(mounts_arr->array[ni], "Read"),
						get_attr_str(mounts_arr->array[ni], "Propagation"));

				array_list_add(listItem->mounts, mount);
			}
			free(mountsObj);
		}
		docker_containers_list_add(clist, listItem);
	}

	free(url);
	return E_SUCCESS;
}

d_err_t make_docker_create_container_params(
		docker_create_container_params** params) {
	docker_create_container_params* p =
			(docker_create_container_params*) malloc(
					sizeof(docker_create_container_params));
	p->hostname = NULL;
	p->domainname = NULL;
	p->user = NULL;
	p->attach_stdin = -1;
	p->attach_stdout = -1;
	p->attach_stderr = -1;
	p->exposed_ports = NULL;
	p->tty = -1;
	p->open_stdin = -1;
	p->stdin_once = -1;
	p->env = NULL;
	p->num_env = -1;
	p->cmd = NULL;
	p->num_cmd = -1;
	p->health_check = NULL;
	p->args_escaped = -1;
	p->image = NULL;
	p->volumes = NULL;
	p->working_dir = NULL;
	p->entrypoint = NULL;
	p->network_disabled = -1;
	p->mac_address = NULL;
	p->on_build = NULL;
	p->num_on_build = -1;
	p->labels = NULL;
	p->stop_signal = NULL;
	p->stop_timeout = -1;
	p->shell = NULL;
	p->host_config = NULL;
	p->network_config = NULL;
	(*params) = p;
	return E_SUCCESS;
}

d_err_t docker_create_container(docker_context* ctx, docker_result** result,
		char** id, docker_create_container_params* params) {
	(*id) = NULL;
	json_object *response_obj = NULL;
	struct http_response_memory chunk;

	json_object* create_obj = json_object_new_object();

	if (params->image != NULL) {
		json_object_object_add(create_obj, "Image",
				json_object_new_string(params->image));
	}
	if (params->cmd != NULL) {
		json_object* cmd_arr = json_object_new_array();
		for (int i = 0; i < params->num_cmd; i++) {
			json_object_array_add(cmd_arr,
					json_object_new_string(params->cmd[i]));
		}
		json_object_object_add(create_obj, "Cmd", cmd_arr);
	}

	docker_api_post(ctx, result, "containers/create", NULL,
			(char*) json_object_to_json_string(create_obj), &chunk,
			&response_obj);

	json_object* idObj;
	if (json_object_object_get_ex(response_obj, "Id", &idObj)) {
		const char* container_id = json_object_get_string(idObj);
		(*id) = (char*) malloc((strlen(container_id) + 1) * sizeof(char));
		strcpy((*id), container_id);
	} else {
		docker_log_debug("Id not found.");
	}
	free(chunk.memory);
	return E_SUCCESS;
}

/**
 * List all processes in a container identified by id.
 *
 * \param ctx is a docker context
 * \param id is the container id
 * \param ps_args is the command line args to be passed to the ps command (can be NULL).
 * \return the process details as docker_container_ps list.
 */
d_err_t docker_process_list_container(docker_context* ctx,
		docker_result** result, docker_container_ps**ps, char* id,
		char* process_args) {
	char* url = create_service_url_id_method(CONTAINER, id, "top");
	docker_log_debug("Top url is %s", url);

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_get(ctx, result, url, NULL, &chunk, &response_obj);

	if (is_ok((*result))) {
		docker_container_ps* p;
		p = (docker_container_ps*) malloc(sizeof(docker_container_ps));
		if (!p) {
			return E_ALLOC_FAILED;
		}
		json_object* titles_obj;
		json_object_object_get_ex(response_obj, "Titles", &titles_obj);
		int num_titles = json_object_array_length(titles_obj);
		p->titles = array_list_new(&free);
		for (int i = 0; i < num_titles; i++) {
			array_list_add(p->titles,
					(char *) json_object_get_string(
							json_object_array_get_idx(titles_obj, i)));
		}

		json_object* processes_obj;
		json_object_object_get_ex(response_obj, "Processes", &processes_obj);
		int num_processes = json_object_array_length(processes_obj);
		p->processes = array_list_new((void (*)(void *)) &array_list_free);
		for (int i = 0; i < num_processes; i++) {
			json_object* process_obj = json_object_array_get_idx(processes_obj,
					i);
			struct array_list* process_arr = array_list_new(&free);
			int num_vals = json_object_array_length(process_obj);
			for (int j = 0; j < num_vals; j++) {
				array_list_add(process_arr,
						(char*) json_object_get_string(
								json_object_array_get_idx(process_obj, j)));
			}
			array_list_add(p->processes, process_arr);
		}
		(*ps) = p;
	} else {
		json_object* msg_obj;
		json_object_object_get_ex(response_obj, "message", &msg_obj);
		(*result)->message = (char *) json_object_to_json_string(msg_obj);
	}
	free(chunk.memory);
	return E_SUCCESS;
}

/**
 * Get the logs for the docker container.
 *
 * \param ctx docker context
 * \param result pointer to docker_result
 * \param log pointer to string to be returned.
 * \param follow - this param has no effect for now, as socket support is not implemented.
 * \param stdout whether to get stdout (>0 means yes)
 * \param stderr whether to get stdin (>0 means yes)
 * \param since time since which the logs are to be fetched (unix timestamp)
 * \param until time till which the logs are to be fetched (unix timestamp)
 * \param timestamps add timestamps to log lines (>0 means yes)
 * \param tail 0 means all, any positive number indicates the number of lines to fetch.
 * \return error code
 */
d_err_t docker_container_logs(docker_context* ctx, docker_result** result,
		char** log, char* id, int follow, int std_out, int std_err, long since,
		long until, int timestamps, int tail) {
	char* method = "/logs";
	char* containers = "containers/";
	char* url = (char*) malloc(
			(strlen(containers) + strlen(id) + strlen(method) + 1)
					* sizeof(char));
	sprintf(url, "%s%s%s", containers, id, method);
	docker_log_debug("Stdout url is %s", url);

	struct array_list* params = array_list_new(
			(void (*)(void *)) &free_url_param);
	url_param* p;

	if (std_out > 0) {
		make_url_param(&p, "stdout", "true");
		array_list_add(params, p);
	}

	if (std_err > 0) {
		make_url_param(&p, "stderr", "true");
		array_list_add(params, p);
	}

	if (since >= 0) {
		char* since_val = (char*) malloc(128 * sizeof(char));
		sprintf(since_val, "%ld", since);
		make_url_param(&p, "since", since_val);
		array_list_add(params, p);
	}

	if (until > 0) {
		char* until_val = (char*) malloc(128 * sizeof(char));
		sprintf(until_val, "%ld", until);
		make_url_param(&p, "until", until_val);
		array_list_add(params, p);
	}

	if (timestamps > 0) {
		make_url_param(&p, "timestamps", "true");
		array_list_add(params, p);
	}

	if (tail > 0) {
		char* tail_val = (char*) malloc(128 * sizeof(char));
		sprintf(tail_val, "%d", tail);
		make_url_param(&p, "tail", tail_val);
		array_list_add(params, p);
	}

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_get(ctx, result, url, params, &chunk, &response_obj);

	free(params);

	(*log) = chunk.memory + 8;
	return E_SUCCESS;
}

///////////// Get Container FS Changes

/**
 * Create a new container change item.
 */
d_err_t make_docker_container_change(docker_container_change** item,
		const char* path, const char* kind) {
	(*item) = (docker_container_change*) malloc(
			sizeof(docker_container_change));
	if (!(*item)) {
		return E_ALLOC_FAILED;
	}
	(*item)->path = make_defensive_copy(path);
	if (kind != NULL) {
		if (strcmp(kind, "0") == 0) {
			(*item)->kind = DOCKER_FS_MODIFIED;
		}
		if (strcmp(kind, "1") == 0) {
			(*item)->kind = DOCKER_FS_ADDED;
		}
		if (strcmp(kind, "2") == 0) {
			(*item)->kind = DOCKER_FS_DELETED;
		}
	}
	return E_SUCCESS;
}

void free_docker_container_change(docker_container_change* item) {
	if (item) {
		if (item->path) {
			free(item->path);
		}
		free(item);
	}
}

d_err_t make_docker_changes_list(docker_changes_list** changes_list) {
	(*changes_list) = array_list_new(
			(void (*)(void *)) &free_docker_container_change);
	return E_SUCCESS;
}

int docker_changes_list_add(docker_changes_list* list,
		docker_container_change* item) {
	return array_list_add(list, item);
}

docker_container_change* docker_changes_list_get_idx(docker_changes_list* list,
		int i) {
	return (docker_container_change*) array_list_get_idx(list, i);
}
int docker_changes_list_length(docker_changes_list* list) {
	return array_list_length(list);
}

/**
 * Get the file system changes for the docker container.
 *
 * \param ctx docker context
 * \param result pointer to docker_result
 * \param changes pointer to struct to be returned.
 * \param id container id
 * \return error code
 */
d_err_t docker_container_changes(docker_context* ctx, docker_result** result,
		docker_changes_list** changes, char* id) {
	char* url = create_service_url_id_method(CONTAINER, id, "changes");

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_get(ctx, result, url, NULL, &chunk, &response_obj);

	if ((json_object_get_type(response_obj) != json_type_null)) {
		docker_log_debug("Response = %s",
				json_object_to_json_string(response_obj));

		make_docker_changes_list(changes);
		for (int i = 0; i < json_object_array_length(response_obj); i++) {
			json_object* change_obj = json_object_array_get_idx(response_obj,
					i);
			docker_container_change* change;
			make_docker_container_change(&change,
					get_attr_str(change_obj, "Path"),
					get_attr_str(change_obj, "Kind"));
			docker_changes_list_add((*changes), change);
		}
	} else {
		docker_log_warn("Response = %s",
				json_object_to_json_string(response_obj));

		(*changes) = NULL;
	}
	return E_SUCCESS;
}

/////// Docker container stats

d_err_t make_docker_container_pids_stats(
		docker_container_pids_stats** pids_stats, int current) {
	(*pids_stats) = (docker_container_pids_stats*) malloc(
			sizeof(docker_container_pids_stats));
	if (!(*pids_stats)) {
		return E_ALLOC_FAILED;
	}
	(*pids_stats)->current = current;
	return E_SUCCESS;
}

void free_docker_container_pids_stats(docker_container_pids_stats* pids) {
	free(pids);
}

d_err_t make_docker_container_net_stats(docker_container_net_stats** net_stats,
		char* name, unsigned long rx_bytes, unsigned long rx_dropped,
		unsigned long rx_errors, unsigned long rx_packets,
		unsigned long tx_bytes, unsigned long tx_dropped,
		unsigned long tx_errors, unsigned long tx_packets) {
	(*net_stats) = (docker_container_net_stats*) malloc(
			sizeof(docker_container_net_stats));
	if (!(*net_stats)) {
		return E_ALLOC_FAILED;
	}
	(*net_stats)->name = make_defensive_copy(name);
	(*net_stats)->rx_bytes = rx_bytes;
	(*net_stats)->rx_dropped = rx_dropped;
	(*net_stats)->rx_errors = rx_errors;
	(*net_stats)->rx_packets = rx_packets;
	(*net_stats)->tx_bytes = tx_bytes;
	(*net_stats)->tx_dropped = tx_dropped;
	(*net_stats)->tx_errors = tx_errors;
	(*net_stats)->tx_packets = tx_packets;
	return E_SUCCESS;
}

void free_docker_container_net_stats(docker_container_net_stats* net_stats) {
	free(net_stats->name);
	free(net_stats);
}

d_err_t make_docker_container_mem_stats(docker_container_mem_stats** mem_stats,
		unsigned long max_usage, unsigned long usage, unsigned long failcnt,
		unsigned long limit) {
	(*mem_stats) = (docker_container_mem_stats*) malloc(
			sizeof(docker_container_mem_stats));
	if (!(*mem_stats)) {
		return E_ALLOC_FAILED;
	}
	(*mem_stats)->max_usage = max_usage;
	(*mem_stats)->usage = usage;
	(*mem_stats)->failcnt = failcnt;
	(*mem_stats)->limit = limit;
	return E_SUCCESS;
}

void free_docker_container_mem_stats(docker_container_mem_stats* mem_stats) {
	free(mem_stats);
}

d_err_t make_docker_container_cpu_stats(docker_container_cpu_stats** cpu_stats,
		unsigned long total_usage, unsigned long usage_in_usermode,
		unsigned long usage_in_kernelmode, unsigned long system_cpu_usage,
		int online_cpus) {
	(*cpu_stats) = (docker_container_cpu_stats*) malloc(
			sizeof(docker_container_cpu_stats));
	if (!(*cpu_stats)) {
		return E_ALLOC_FAILED;
	}
	(*cpu_stats)->total_usage = total_usage;
	(*cpu_stats)->usage_in_usermode = usage_in_usermode;
	(*cpu_stats)->usage_in_kernelmode = usage_in_kernelmode;
	(*cpu_stats)->system_cpu_usage = system_cpu_usage;
	(*cpu_stats)->percpu_usage = array_list_new(NULL);
	(*cpu_stats)->online_cpus = online_cpus;
	return E_SUCCESS;
}

void free_docker_container_cpu_stats(docker_container_cpu_stats* cpu_stats) {
	free(cpu_stats->percpu_usage);
	free(cpu_stats);
}

d_err_t make_docker_container_stats(docker_container_stats** stats,
		struct tm* read, docker_container_pids_stats* pid_stats,
		docker_container_mem_stats* mem_stats,
		docker_container_cpu_stats* cpu_stats,
		docker_container_cpu_stats* precpu_stats) {
	(*stats) = (docker_container_stats*) malloc(sizeof(docker_container_stats));
	if (!(*stats)) {
		return E_ALLOC_FAILED;
	}
	(*stats)->read = read;
	(*stats)->pid_stats = pid_stats;
	(*stats)->mem_stats = mem_stats;
	(*stats)->cpu_stats = cpu_stats;
	(*stats)->precpu_stats = precpu_stats;
	(*stats)->net_stats_list = array_list_new(
			(void (*)(void *)) &free_docker_container_net_stats);
	return E_SUCCESS;
}

//TODO: Bug, in free'ing the structs in this function, fix this.
void free_docker_container_stats(docker_container_stats* stats) {
	free(stats->read);
	free(stats->pid_stats);
	free(stats->mem_stats);
	free(stats->cpu_stats);
	free(stats->precpu_stats);
	array_list_free(stats->net_stats_list);
	free(stats);
}

void get_cpu_stats_for_name(json_object* response_obj, char* stats_obj_name,
		docker_container_cpu_stats** cpu_stats_ret) {
	docker_container_cpu_stats* cpu_stats;
	json_object* cpu_stats_obj = NULL;
	json_object_object_get_ex(response_obj, stats_obj_name, &cpu_stats_obj);
	if (cpu_stats_obj) {
		json_object* cpu_stats_cpu_usage_obj = NULL;
		json_object_object_get_ex(cpu_stats_obj, "cpu_usage",
				&cpu_stats_cpu_usage_obj);
		if (cpu_stats_cpu_usage_obj) {
			make_docker_container_cpu_stats(&cpu_stats,
					get_attr_unsigned_long(cpu_stats_cpu_usage_obj,
							"total_usage"),
					get_attr_unsigned_long(cpu_stats_cpu_usage_obj,
							"usage_in_usermode"),
					get_attr_unsigned_long(cpu_stats_cpu_usage_obj,
							"usage_in_kernelmode"),
					get_attr_unsigned_long(cpu_stats_obj, "system_cpu_usage"),
					get_attr_int(cpu_stats_obj, "online_cpus"));
			json_object* percpu_usage_obj = NULL;
			json_object_object_get_ex(cpu_stats_cpu_usage_obj, "percpu_usage",
					&percpu_usage_obj);
			if (percpu_usage_obj) {
				int percpu_usage_len = json_object_array_length(
						percpu_usage_obj);
				for (int i = 0; i < percpu_usage_len; i++) {
					//FIXME: value is stored incorrect.
					unsigned long* pcpu_usg;
					*pcpu_usg = json_object_get_int64(
							(json_object_array_get_idx(percpu_usage_obj, i)));
					array_list_add(cpu_stats->percpu_usage, pcpu_usg);
				}
			}
		}
	}
	(*cpu_stats_ret) = cpu_stats;
}

/**
 * Get stats from a running container. (the non-streaming version)
 *
 * \param ctx docker context
 * \param result pointer to docker_result
 * \param stats the stats object to return
 * \param id container id
 * \return error code
 */
d_err_t docker_container_get_stats(docker_context* ctx, docker_result** result,
		docker_container_stats** stats, char* id) {
	if (id == NULL || strlen(id) == 0) {
		return E_INVALID_INPUT;
	}

	char* url = create_service_url_id_method(CONTAINER, id, "stats");
	struct array_list* params = array_list_new(
			(void (*)(void *)) &free_url_param);
	url_param* p;
	make_url_param(&p, "stream", make_defensive_copy("false"));
	array_list_add(params, p);

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_get(ctx, result, url, params, &chunk, &response_obj);

	if ((*result) && (*result)->http_error_code) {
		if (response_obj) {
			struct tm* read_time = (struct tm*) calloc(1, sizeof(struct tm));
			if (!read_time) {
				return E_ALLOC_FAILED;
			}
			parse_docker_stats_readtime(get_attr_str(response_obj, "read"),
					read_time);

			json_object* pid_stats_obj = NULL;
			docker_container_pids_stats* pid_stats = NULL;
			json_object_object_get_ex(response_obj, "pid_stats",
					&pid_stats_obj);
			if (pid_stats_obj) {
				make_docker_container_pids_stats(&pid_stats,
						get_attr_int(pid_stats_obj, "current"));
			}

			json_object* mem_stats_obj = NULL;
			docker_container_mem_stats* mem_stats = NULL;
			json_object_object_get_ex(response_obj, "memory_stats",
					&mem_stats_obj);
			if (mem_stats_obj) {
				make_docker_container_mem_stats(&mem_stats,
						get_attr_unsigned_long(mem_stats_obj, "max_usage"),
						get_attr_unsigned_long(mem_stats_obj, "usage"),
						get_attr_unsigned_long(mem_stats_obj, "failcnt"),
						get_attr_unsigned_long(mem_stats_obj, "limit"));
			}

			docker_container_cpu_stats* cpu_stats = NULL;
			char* stats_obj_name = "cpu_stats";
			get_cpu_stats_for_name(response_obj, stats_obj_name, &cpu_stats);

			docker_container_cpu_stats* precpu_stats = NULL;
			stats_obj_name = "precpu_stats";
			get_cpu_stats_for_name(response_obj, stats_obj_name, &precpu_stats);

			docker_container_stats* s = NULL;
			make_docker_container_stats(&s, read_time, pid_stats, mem_stats,
					cpu_stats, precpu_stats);
			json_object* net_stats_group_obj = NULL;
			json_object_object_get_ex(response_obj, "networks",
					&net_stats_group_obj);
			if (net_stats_group_obj) {
				json_object_object_foreach(net_stats_group_obj, k, v)
				{
					docker_container_net_stats* net_stats;
					make_docker_container_net_stats(&net_stats, k,
							get_attr_unsigned_long(v, "rx_bytes"),
							get_attr_unsigned_long(v, "rx_dropped"),
							get_attr_unsigned_long(v, "rx_errors"),
							get_attr_unsigned_long(v, "rx_packets"),
							get_attr_unsigned_long(v, "tx_bytes"),
							get_attr_unsigned_long(v, "tx_dropped"),
							get_attr_unsigned_long(v, "tx_errors"),
							get_attr_unsigned_long(v, "tx_packets"));
					array_list_add(s->net_stats_list, net_stats);
				}
			}
			(*stats) = s;
		}
	}

	return E_SUCCESS;
}

void parse_container_stats_cb(char* msg, void* cb, void* cbargs) {
	void (*docker_container_stats_cb)(docker_container_stats*,
			void*) = (void (*)(docker_container_stats*, void*))cb;
	if (msg) {
		if(docker_container_stats_cb) {
			json_object* response_obj = json_tokener_parse(msg);
			if (response_obj) {
				struct tm* read_time = (struct tm*) calloc(1, sizeof(struct tm));
				if (!read_time) {
					return;
				}
				parse_docker_stats_readtime(get_attr_str(response_obj, "read"),
						read_time);

				json_object* pid_stats_obj = NULL;
				docker_container_pids_stats* pid_stats = NULL;
				json_object_object_get_ex(response_obj, "pid_stats",
						&pid_stats_obj);
				if (pid_stats_obj) {
					make_docker_container_pids_stats(&pid_stats,
							get_attr_int(pid_stats_obj, "current"));
				}

				json_object* mem_stats_obj = NULL;
				docker_container_mem_stats* mem_stats = NULL;
				json_object_object_get_ex(response_obj, "memory_stats",
						&mem_stats_obj);
				if (mem_stats_obj) {
					make_docker_container_mem_stats(&mem_stats,
							get_attr_unsigned_long(mem_stats_obj, "max_usage"),
							get_attr_unsigned_long(mem_stats_obj, "usage"),
							get_attr_unsigned_long(mem_stats_obj, "failcnt"),
							get_attr_unsigned_long(mem_stats_obj, "limit"));
				}

				docker_container_cpu_stats* cpu_stats = NULL;
				char* stats_obj_name = "cpu_stats";
				get_cpu_stats_for_name(response_obj, stats_obj_name, &cpu_stats);

				docker_container_cpu_stats* precpu_stats = NULL;
				stats_obj_name = "precpu_stats";
				get_cpu_stats_for_name(response_obj, stats_obj_name, &precpu_stats);

				docker_container_stats* s = NULL;
				make_docker_container_stats(&s, read_time, pid_stats, mem_stats,
						cpu_stats, precpu_stats);

				json_object* net_stats_group_obj = NULL;
				json_object_object_get_ex(response_obj, "networks",
						&net_stats_group_obj);
				if (net_stats_group_obj) {
					json_object_object_foreach(net_stats_group_obj, k, v)
					{
						docker_container_net_stats* net_stats;
						make_docker_container_net_stats(&net_stats, k,
								get_attr_unsigned_long(v, "rx_bytes"),
								get_attr_unsigned_long(v, "rx_dropped"),
								get_attr_unsigned_long(v, "rx_errors"),
								get_attr_unsigned_long(v, "rx_packets"),
								get_attr_unsigned_long(v, "tx_bytes"),
								get_attr_unsigned_long(v, "tx_dropped"),
								get_attr_unsigned_long(v, "tx_errors"),
								get_attr_unsigned_long(v, "tx_packets"));
						array_list_add(s->net_stats_list, net_stats);
					}
				}
				docker_container_stats_cb(s, cbargs);
			}
		}
	}
}
/**
 * Get stats from a running container. (the streaming version)
 *
 * \param ctx docker context
 * \param result pointer to docker_result
 * \param docker_container_stats_cb the callback which receives the stats object, and any client args
 * \param cbargs client args to be passed on to the callback (closure)
 * \param id container id
 * \return error code
 */
d_err_t docker_container_get_stats_cb(docker_context* ctx,
		docker_result** result,
		void (*docker_container_stats_cb)(docker_container_stats* stats,
				void* cbargs), void* cbargs, char* id) {
	if (id == NULL || strlen(id) == 0) {
		return E_INVALID_INPUT;
	}

	char* url = create_service_url_id_method(CONTAINER, id, "stats");
	struct array_list* params = array_list_new(
			(void (*)(void *)) &free_url_param);
	url_param* p;
	make_url_param(&p, "stream", make_defensive_copy("true"));
	array_list_add(params, p);

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_get_cb(ctx, result, url, params, &chunk, &response_obj,
			&parse_container_stats_cb, docker_container_stats_cb, cbargs);

	return E_SUCCESS;
}

float docker_container_stats_get_cpu_usage_percent(
		docker_container_stats* stats) {
	float cpu_percent = 0.0;
	docker_container_cpu_stats* cpu_stats = stats->cpu_stats;
	docker_container_cpu_stats* precpu_stats = stats->precpu_stats;
	int cpu_count = cpu_stats->online_cpus;

	unsigned long cpu_delta = cpu_stats->total_usage
			- precpu_stats->total_usage;
	unsigned long sys_delta = cpu_stats->system_cpu_usage
			- precpu_stats->system_cpu_usage;
	if (sys_delta > 0) {
		cpu_percent = (100.0 * cpu_delta * cpu_count) / sys_delta;
	}
	return cpu_percent;
}

/**
 * Start a container
 *
 * \param ctx docker context
 * \param result pointer to docker_result
 * \param id container id
 * \param detachKeys (optional, pass NULL if not needed) key combination for detaching a container.
 * \return error code
 */
d_err_t docker_start_container(docker_context* ctx, docker_result** result,
		char* id, char* detachKeys) {
	char* url = create_service_url_id_method(CONTAINER, id, "start");

	struct array_list* params = array_list_new(
			(void (*)(void *)) &free_url_param);
	url_param* p;

	if (detachKeys != NULL) {
		make_url_param(&p, "detachKeys", detachKeys);
		array_list_add(params, p);
	}

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_post(ctx, result, url, params, "", &chunk, &response_obj);

	free(params);
	return E_SUCCESS;
}

/**
 * Stop a container
 *
 * \param ctx docker context
 * \param result pointer to docker_result
 * \param id container id
 * \param t number of seconds to wait before killing the container
 * \return error code
 */
d_err_t docker_stop_container(docker_context* ctx, docker_result** result,
		char* id, int t) {
	char* url = create_service_url_id_method(CONTAINER, id, "stop");

	struct array_list* params = array_list_new(
			(void (*)(void *)) &free_url_param);
	url_param* p;

	if (t > 0) {
		char* tstr = (char*) malloc(sizeof(char) * 128);
		sprintf(tstr, "%d", t);
		make_url_param(&p, "t", tstr);
		array_list_add(params, p);
	}

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_post(ctx, result, url, params, "", &chunk, &response_obj);

	if ((*result)->http_error_code == 304) {
		(*result)->message = make_defensive_copy(
				"container is already stopped.");
	}

	if ((*result)->http_error_code == 404) {
		(*result)->message = make_defensive_copy("container not found.");
	}

	free(params);
	return E_SUCCESS;
}

/**
 * Restart a container
 *
 * \param ctx docker context
 * \param result pointer to docker_result
 * \param id container id
 * \param t number of seconds to wait before killing the container
 * \return error code
 */
d_err_t docker_restart_container(docker_context* ctx, docker_result** result,
		char* id, int t) {
	char* url = create_service_url_id_method(CONTAINER, id, "restart");

	struct array_list* params = array_list_new(
			(void (*)(void *)) &free_url_param);
	url_param* p;

	if (t > 0) {
		char* tstr = (char*) malloc(sizeof(char) * 128);
		sprintf(tstr, "%d", t);
		make_url_param(&p, "t", tstr);
		array_list_add(params, p);
	}

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_post(ctx, result, url, params, "", &chunk, &response_obj);

	if ((*result)->http_error_code == 404) {
		(*result)->message = make_defensive_copy("container not found.");
	}

	free(params);
	return E_SUCCESS;
}

/**
 * Kill a container
 *
 * \param ctx docker context
 * \param result pointer to docker_result
 * \param id container id
 * \param signal (optional - NULL for default i.e. SIGKILL) signal name to send
 * \return error code
 */
d_err_t docker_kill_container(docker_context* ctx, docker_result** result,
		char* id, char* signal) {
	char* url = create_service_url_id_method(CONTAINER, id, "kill");

	struct array_list* params = array_list_new(
			(void (*)(void *)) &free_url_param);
	url_param* p;

	if (signal != NULL) {
		make_url_param(&p, "signal", make_defensive_copy(signal));
		array_list_add(params, p);
	}

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_post(ctx, result, url, params, "", &chunk, &response_obj);

	if ((*result)->http_error_code == 404) {
		(*result)->message = make_defensive_copy("container not found.");
	}

	if ((*result)->http_error_code == 409) {
		(*result)->message = make_defensive_copy("container is not running.");
	}

	free(params);
	return E_SUCCESS;
}

/**
 * Rename a container
 *
 * \param ctx docker context
 * \param result pointer to docker_result
 * \param id container id
 * \param name new name for the container
 * \return error code
 */
d_err_t docker_rename_container(docker_context* ctx, docker_result** result,
		char* id, char* name) {
	char* url = create_service_url_id_method(CONTAINER, id, "rename");

	struct array_list* params = array_list_new(
			(void (*)(void *)) &free_url_param);
	url_param* p;

	if (name != NULL) {
		make_url_param(&p, "name", make_defensive_copy(name));
		array_list_add(params, p);
	}

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_post(ctx, result, url, params, "", &chunk, &response_obj);

	if ((*result)->http_error_code == 404) {
		(*result)->message = make_defensive_copy("container not found.");
	}

	if ((*result)->http_error_code == 409) {
		(*result)->message = make_defensive_copy("name is already in use");
	}

	free(params);
	return E_SUCCESS;
}

/**
 * Pause a container
 *
 * \param ctx docker context
 * \param result pointer to docker_result
 * \param id container id
 * \return error code
 */
d_err_t docker_pause_container(docker_context* ctx, docker_result** result,
		char* id) {
	char* url = create_service_url_id_method(CONTAINER, id, "pause");

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_post(ctx, result, url, NULL, "", &chunk, &response_obj);

	if ((*result)->http_error_code == 404) {
		(*result)->message = make_defensive_copy("container not found.");
	}
	return E_SUCCESS;
}

/**
 * Unpause a container
 *
 * \param ctx docker context
 * \param result pointer to docker_result
 * \param id container id
 * \return error code
 */
d_err_t docker_unpause_container(docker_context* ctx, docker_result** result,
		char* id) {
	char* url = create_service_url_id_method(CONTAINER, id, "unpause");

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_post(ctx, result, url, NULL, "", &chunk, &response_obj);

	if ((*result)->http_error_code == 404) {
		(*result)->message = make_defensive_copy("container not found.");
	}
	return E_SUCCESS;
}

/**
 * Wait for a container
 *
 * \param ctx docker context
 * \param result pointer to docker_result
 * \param id container id
 * \param condition (optional - NULL for default "not-running") condition to wait for
 * \return error code
 */
d_err_t docker_wait_container(docker_context* ctx, docker_result** result,
		char* id, char* condition) {
	char* url = create_service_url_id_method(CONTAINER, id, "wait");

	struct array_list* params = array_list_new(
			(void (*)(void *)) &free_url_param);
	url_param* p;

	if (condition != NULL) {
		make_url_param(&p, "condition", make_defensive_copy(condition));
		array_list_add(params, p);
	}

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_post(ctx, result, url, NULL, "", &chunk, &response_obj);

	//TODO free response_obj
	return E_SUCCESS;
}
