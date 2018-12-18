/*
 * docker_containers.c
 *
 *  Created on: 11-Dec-2018
 *      Author: abhishek
 */

#include "docker_containers.h"

#include <json-c/arraylist.h>
#include <json-c/json_object.h>
#include <json-c/json_tokener.h>
#include <json-c/linkhash.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "log.h"
#include "docker_connection_util.h"

#define ADD_FILTER_STR_ATTR(name) \
	void containers_filter_add_ ## name(docker_containers_list_filter* filter, char* val) { \
		if (filter->num_ ## name == 0) { \
			filter->name = (char**) malloc(128 * sizeof(char*)); \
		} \
		filter->name[filter->num_ ## name] = val; \
		filter->num_ ## name += 1; \
	}

#define ADD_FILTER_INT_ATTR(name) \
	void containers_filter_add_ ## name(docker_containers_list_filter* filter, int val) { \
		if (filter->num_ ## name == 0) { \
			filter->name = (int*) malloc(128 * sizeof(int)); \
		} \
		filter->name[filter->num_ ## name] = val; \
		filter->num_ ## name += 1; \
	}

#define DOCKER_CONTAINER_GETTER_IMPL(object, type, name) \
	type docker_container_ ## object ## _get_ ## name(docker_container_ ## object* object) { \
		return object->name; \
	} \


#define DOCKER_CONTAINER_GETTER_ARR_ADD_IMPL(object, type, name) \
	int docker_container_ ## object ## _get_ ## name ## _add(docker_container_ ## object* object, type data) { \
		return array_list_add(object->name, (void*) data); \
	} \

#define DOCKER_CONTAINER_GETTER_ARR_LEN_IMPL(object, name) \
	int docker_container_ ## object ## _get_ ## name ##_length(docker_container_ ## object* object) { \
		return array_list_length(object->name); \
	} \

#define DOCKER_CONTAINER_GETTER_ARR_GET_IDX_IMPL(object, type, name) \
	type docker_container_ ## object ## _get_ ## name ## _get_idx(docker_container_ ## object* object, int i) { \
		return (type) array_list_get_idx(object->name, i); \
	} \

char* make_defensive_copy(const char* from) {
	char* to = NULL;
	if (strlen(from) > 0) {
		to = (char*) malloc((strlen(from) + 1) * sizeof(char));
		strcpy(to, from);
	}
	return to;
}

/**
 * Create a docker_container_ports instance.
 *
 * \param ports the instance to create.
 * \param priv the private port
 * \param pub the public port
 * \param type port type
 * \return error code
 */
error_t make_docker_container_ports(docker_container_ports** ports, long priv,
		long pub, char* type) {
	(*ports) = (docker_container_ports*) malloc(sizeof(docker_container_ports));
	if (!(*ports)) {
		return E_ALLOC_FAILED;
	}
	(*ports)->private_port = priv;
	(*ports)->public_port = pub;
	if (strcasecmp(type, "tcp") == 0) {
		(*ports)->type = CONTAINER_PORT_TYPE_TCP;
	}
	return E_SUCCESS;
}

void free_docker_container_ports(docker_container_ports* ports) {
	free(ports);
}

DOCKER_CONTAINER_GETTER_IMPL(ports, long, public_port)
DOCKER_CONTAINER_GETTER_IMPL(ports, long, private_port)
DOCKER_CONTAINER_GETTER_IMPL(ports, docker_container_port_type, type)

/**
 * Create a new label.
 *
 * \param label ptr to return value.
 * \param key
 * \param value
 * \return error code
 */
error_t make_docker_container_label(docker_container_label** label,
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

DOCKER_CONTAINER_GETTER_IMPL(label, char*, key)
DOCKER_CONTAINER_GETTER_IMPL(label, char*, value)

/**
 * Create a new host config
 *
 * \param pointer to return instance
 * \param network_mode
 * \return error_code
 */
error_t make_docker_container_host_config(
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

DOCKER_CONTAINER_GETTER_IMPL(host_config, char*, network_mode)

/**
 * Create a new network settings item
 */
error_t make_docker_container_network_settings_item(
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

DOCKER_CONTAINER_GETTER_IMPL(network_settings_item, char*, name)
DOCKER_CONTAINER_GETTER_IMPL(network_settings_item, char*, network_id)
DOCKER_CONTAINER_GETTER_IMPL(network_settings_item, char*, endpoint_id)
DOCKER_CONTAINER_GETTER_IMPL(network_settings_item, char*, gateway)
DOCKER_CONTAINER_GETTER_IMPL(network_settings_item, char*, ip_address)
DOCKER_CONTAINER_GETTER_IMPL(network_settings_item, int, ip_prefix_len)
DOCKER_CONTAINER_GETTER_IMPL(network_settings_item, char*, ipv6_gateway)
DOCKER_CONTAINER_GETTER_IMPL(network_settings_item, char*, global_ipv6_address)
DOCKER_CONTAINER_GETTER_IMPL(network_settings_item, int, global_ipv6_prefix_len)
DOCKER_CONTAINER_GETTER_IMPL(network_settings_item, char*, mac_address)

/**
 * Create a new mount object
 */
error_t make_docker_container_mount(docker_container_mount** mount,
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

DOCKER_CONTAINER_GETTER_IMPL(mount, char*, name)
DOCKER_CONTAINER_GETTER_IMPL(mount, char*, type)
DOCKER_CONTAINER_GETTER_IMPL(mount, char*, source)
DOCKER_CONTAINER_GETTER_IMPL(mount, char*, destination)
DOCKER_CONTAINER_GETTER_IMPL(mount, char*, driver)
DOCKER_CONTAINER_GETTER_IMPL(mount, char*, mode)
DOCKER_CONTAINER_GETTER_IMPL(mount, int, rw)
DOCKER_CONTAINER_GETTER_IMPL(mount, char*, propagation)

/**
 * Create a new containers list item.
 */
error_t make_docker_containers_list_item(docker_container_list_item** item,
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
}

DOCKER_CONTAINER_GETTER_IMPL(list_item, char*, id)
DOCKER_CONTAINER_GETTER_IMPL(list_item, char*, image)
DOCKER_CONTAINER_GETTER_IMPL(list_item, char*, image_id)
DOCKER_CONTAINER_GETTER_IMPL(list_item, char*, command)
DOCKER_CONTAINER_GETTER_IMPL(list_item, long long, created)
DOCKER_CONTAINER_GETTER_IMPL(list_item, char*, state)
DOCKER_CONTAINER_GETTER_IMPL(list_item, char*, status)
DOCKER_CONTAINER_GETTER_IMPL(list_item, long long, size_rw)
DOCKER_CONTAINER_GETTER_IMPL(list_item, long long, size_root_fs)

DOCKER_CONTAINER_GETTER_ARR_ADD_IMPL(list_item, char*, names)
DOCKER_CONTAINER_GETTER_ARR_LEN_IMPL(list_item, names)
DOCKER_CONTAINER_GETTER_ARR_GET_IDX_IMPL(list_item, char*, names)

DOCKER_CONTAINER_GETTER_ARR_ADD_IMPL(list_item, docker_container_ports*, ports)
DOCKER_CONTAINER_GETTER_ARR_LEN_IMPL(list_item, ports)
DOCKER_CONTAINER_GETTER_ARR_GET_IDX_IMPL(list_item, docker_container_ports*,
		ports)

DOCKER_CONTAINER_GETTER_ARR_ADD_IMPL(list_item, docker_container_label*, labels)
DOCKER_CONTAINER_GETTER_ARR_LEN_IMPL(list_item, labels)
DOCKER_CONTAINER_GETTER_ARR_GET_IDX_IMPL(list_item, docker_container_label*,
		labels)

DOCKER_CONTAINER_GETTER_ARR_ADD_IMPL(list_item,
		docker_container_network_settings_item*, network_settings)
DOCKER_CONTAINER_GETTER_ARR_LEN_IMPL(list_item, network_settings)
DOCKER_CONTAINER_GETTER_ARR_GET_IDX_IMPL(list_item,
		docker_container_network_settings_item*, network_settings)

DOCKER_CONTAINER_GETTER_ARR_ADD_IMPL(list_item, docker_container_mount*, mounts)
DOCKER_CONTAINER_GETTER_ARR_LEN_IMPL(list_item, mounts)
DOCKER_CONTAINER_GETTER_ARR_GET_IDX_IMPL(list_item, docker_container_mount*,
		mounts)

char* get_attr_str(json_object* obj, char* name) {
	json_object* extractObj;
	char* attr = NULL;
	if (json_object_object_get_ex(obj, name, &extractObj)) {
		attr = (char*) json_object_get_string(extractObj);
		free(extractObj);
	}
	docker_log_debug("%s is |%s|.", name, attr);
	return attr;
}

int get_attr_int(json_object* obj, char* name) {
	json_object* extractObj;
	int attr = -1;
	if (json_object_object_get_ex(obj, name, &extractObj)) {
		sscanf(json_object_get_string(extractObj), "%d", &attr);
		free(extractObj);
	}
	docker_log_debug("%s is |%d|.", name, attr);
	return attr;
}

long long get_attr_long_long(json_object* obj, char* name) {
	json_object* extractObj;
	long long attr = -1;
	if (json_object_object_get_ex(obj, name, &extractObj)) {
		sscanf(json_object_get_string(extractObj), "%lld", &attr);
		free(extractObj);
	}
	docker_log_debug("%s is |%lld|.", name, attr);
	return attr;
}

error_t make_docker_containers_list_filter(docker_containers_list_filter** f) {
	(*f) = (docker_containers_list_filter*) malloc(
			sizeof(docker_containers_list_filter));
	(*f)->num_ancestor = 0;
	(*f)->num_before = 0;
	(*f)->num_expose = 0;
	(*f)->num_exited = 0;
	(*f)->num_health = 0;
	(*f)->num_id = 0;
	(*f)->num_isolation = 0;
	(*f)->num_is_task = 0;
	(*f)->num_label = 0;
	(*f)->num_name = 0;
	(*f)->num_network = 0;
	(*f)->num_publish = 0;
	(*f)->num_since = 0;
	(*f)->num_status = 0;
	(*f)->num_volume = 0;
	return E_SUCCESS;
}

ADD_FILTER_STR_ATTR(ancestor)
ADD_FILTER_STR_ATTR(before)
ADD_FILTER_STR_ATTR(expose)
ADD_FILTER_INT_ATTR(exited)
ADD_FILTER_STR_ATTR(health)
ADD_FILTER_STR_ATTR(id)
ADD_FILTER_STR_ATTR(isolation)
ADD_FILTER_INT_ATTR(is_task)
ADD_FILTER_STR_ATTR(label)
ADD_FILTER_STR_ATTR(name)
ADD_FILTER_STR_ATTR(network)
ADD_FILTER_STR_ATTR(publish)
ADD_FILTER_STR_ATTR(since)
ADD_FILTER_STR_ATTR(status)
ADD_FILTER_STR_ATTR(volume)

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

error_t docker_container_list(docker_context* ctx, docker_result** result,
		struct array_list** container_list, int all, int limit, int size,
		docker_containers_list_filter* filters) {
	char* method = "json";
	char* containers = "containers/";
	char* url = (char*) malloc(
			(strlen(containers) + strlen(method) + 1) * sizeof(char));
	sprintf(url, "%s%s", containers, method);
	docker_log_debug("List url is %s\n", url);

	url_param** params = (url_param**) malloc(sizeof(url_param*) * 4);
	int num_params = 0;

	if (all > 0) {
		params[num_params] = (url_param*) malloc(sizeof(url_param));
		params[num_params]->k = "all";
		params[num_params]->v = "true";
		num_params++;
	}

	if (limit > 0) {
		params[num_params] = (url_param*) malloc(sizeof(url_param));
		params[num_params]->k = "limit";
		params[num_params]->v = (char*) malloc(128 * sizeof(char));
		sprintf(params[num_params]->v, "%d", limit);
		num_params++;
	}

	if (size > 0) {
		params[num_params] = (url_param*) malloc(sizeof(url_param));
		params[num_params]->k = "size";
		params[num_params]->v = "true";
		num_params++;
	}

	if (filters) {
		params[num_params] = (url_param*) malloc(sizeof(url_param));
		params[num_params]->k = "filters";
		json_object* fobj = json_object_new_object();

		//set filter attributes
		extract_filter_field_char(fobj, "ancestor", filters->num_ancestor,
				filters->ancestor);
		extract_filter_field_char(fobj, "before", filters->num_before,
				filters->before);
		extract_filter_field_char(fobj, "expose", filters->num_expose,
				filters->expose);
		extract_filter_field_int(fobj, "exited", filters->num_exited,
				filters->exited);
		extract_filter_field_char(fobj, "health", filters->num_health,
				filters->health);
		extract_filter_field_char(fobj, "id", filters->num_id, filters->id);
		extract_filter_field_char(fobj, "isolation", filters->num_isolation,
				filters->isolation);
		extract_filter_field_int(fobj, "is_task", filters->num_is_task,
				filters->is_task);
		extract_filter_field_char(fobj, "label", filters->num_label,
				filters->label);
		extract_filter_field_char(fobj, "name", filters->num_name,
				filters->name);
		extract_filter_field_char(fobj, "network", filters->num_network,
				filters->network);
		extract_filter_field_char(fobj, "publish", filters->num_publish,
				filters->publish);
		extract_filter_field_char(fobj, "since", filters->num_since,
				filters->since);
		extract_filter_field_char(fobj, "status", filters->num_status,
				filters->status);
		extract_filter_field_char(fobj, "volume", filters->num_volume,
				filters->volume);

		params[num_params]->v = (char *) json_object_to_json_string(fobj);
		num_params++;
	}

	json_object *new_obj;
	struct MemoryStruct chunk;
	docker_api_get(ctx, result, url, params, num_params, &chunk);
	free(params);

	//need to skip 8 bytes of binary junk
	//log_debug("Output is \n%s\n", chunk.memory);

	new_obj = json_tokener_parse(chunk.memory);
	//log_debug("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
	struct array_list* containers_arr = json_object_get_array(new_obj);
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

		array_list_add(clist, listItem);

		json_object* namesObj;
		if (json_object_object_get_ex(containers_arr->array[i], "Names",
				&namesObj)) {
			struct array_list* names_arr = json_object_get_array(namesObj);
			for (int ni = 0; ni < names_arr->length; ni++) {
				docker_container_list_item_get_names_add(listItem,
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
						get_attr_int(ports_arr->array[ni], "PrivatePort"),
						get_attr_int(ports_arr->array[ni], "PublicPort"),
						get_attr_str(ports_arr->array[ni], "Type"));
				docker_container_list_item_get_ports_add(listItem, ports);
			}
		}
		free(portsObj);

		json_object* labelsObj;
		if (json_object_object_get_ex(containers_arr->array[i], "Labels",
				&labelsObj)) {
			json_object_object_foreach(labelsObj, key1, val1)
			{
				docker_container_label* label;
				make_docker_container_label(&label, key1, json_object_get_string((json_object *) val1));
				docker_log_debug("Label [%s] = [%s]",
						docker_container_label_get_key(label),
						docker_container_label_get_value(label));
			}
		}
		free(labelsObj);

		json_object* networkSettingsObj;
		if (json_object_object_get_ex(containers_arr->array[i],
				"NetworkSettings", &networkSettingsObj)) {
			json_object* networksObj;
			if (json_object_object_get_ex(networkSettingsObj, "Networks",
					&networksObj)) {
//				listItem->network_settings =
//						(docker_container_network_settings*) malloc(
//								sizeof(docker_container_network_settings));
//				listItem->network_settings->num_network_items = 0;
//				json_object_object_foreach(networksObj, key, val) {
//					listItem->network_settings->num_network_items += 1;
//				}
//				docker_log_debug("Num networks is %d",
//						listItem->network_settings->num_network_items);
//				listItem->network_settings->network_items =
//						(docker_container_network_settings_item**) malloc(
//								listItem->network_settings->num_network_items
//										* sizeof(docker_container_network_settings_item*));
//				int ns_count = 0;
//				json_object_object_foreach(networksObj, k, v) {
//					listItem->network_settings->network_items[ns_count] =
//							(docker_container_network_settings_item*) malloc(
//									sizeof(docker_container_network_settings_item));
//					listItem->network_settings->network_items[ns_count]->name =
//							make_defensive_copy(k);
//					listItem->network_settings->network_items[ns_count]->item =
//							(docker_container_network_settings_network_item*) malloc(
//									sizeof(docker_container_network_settings_network_item));
//					listItem->network_settings->network_items[ns_count]->item->network_id =
//							get_attr_str(v, "NetworkID");
//					listItem->network_settings->network_items[ns_count]->item->endpoint_id =
//							get_attr_str(v, "EndpointID");
//					listItem->network_settings->network_items[ns_count]->item->gateway =
//							get_attr_str(v, "Gateway");
//					listItem->network_settings->network_items[ns_count]->item->ip_address =
//							get_attr_str(v, "IPAddress");
//					listItem->network_settings->network_items[ns_count]->item->ip_prefix_len =
//							get_attr_int(v, "IPPrefixLen");
//					listItem->network_settings->network_items[ns_count]->item->ipv6_gateway =
//							get_attr_str(v, "IPv6Gateway");
//					listItem->network_settings->network_items[ns_count]->item->global_ipv6_address =
//							get_attr_str(v, "GlobalIPv6Address");
//					listItem->network_settings->network_items[ns_count]->item->global_ipv6_prefix_len =
//							get_attr_int(v, "GlobalIPv6PrefixLen");
//					listItem->network_settings->network_items[ns_count]->item->mac_address =
//							get_attr_str(v, "MacAddress");
//					ns_count++;
//				}
//				free(networksObj);
			}
			free(networkSettingsObj);
		}

		json_object* mountsObj;
//		if (json_object_object_get_ex(containers_arr->array[i], "Mounts",
//				&mountsObj)) {
//			struct array_list* mounts_arr = json_object_get_array(mountsObj);
//			listItem->mounts = (docker_container_mount**) malloc(
//					mounts_arr->length * sizeof(docker_container_mount*));
//			for (int ni = 0; ni < mounts_arr->length; ni++) {
//				listItem->mounts[ni] = (docker_container_mount*) malloc(
//						sizeof(docker_container_mount));
//				listItem->mounts[ni]->name = get_attr_str(mounts_arr->array[ni],
//						"Name");
//				listItem->mounts[ni]->type = get_attr_str(mounts_arr->array[ni],
//						"Type");
//				listItem->mounts[ni]->source = get_attr_str(
//						mounts_arr->array[ni], "Source");
//				listItem->mounts[ni]->destination = get_attr_str(
//						mounts_arr->array[ni], "Destination");
//				listItem->mounts[ni]->driver = get_attr_str(
//						mounts_arr->array[ni], "Driver");
//				listItem->mounts[ni]->mode = get_attr_str(mounts_arr->array[ni],
//						"Mode");
//				listItem->mounts[ni]->rw = get_attr_int(mounts_arr->array[ni],
//						"Read");
//				listItem->mounts[ni]->propagation = get_attr_str(
//						mounts_arr->array[ni], "Propagation");
//			}
//			listItem->num_names = mounts_arr->length;
//			free(mountsObj);
//		}
	}

	free(url);
	return E_SUCCESS;
}

error_t make_docker_create_container_params(
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

error_t docker_create_container(docker_context* ctx, docker_result** result,
		char** id, docker_create_container_params* params) {
	(*id) = NULL;
	json_object *new_obj;
	struct MemoryStruct chunk;

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

	docker_api_post(ctx, result, "containers/create", NULL, 0,
			(char*) json_object_to_json_string(create_obj), &chunk);

	new_obj = json_tokener_parse(chunk.memory);
	docker_log_debug("Response = %s", json_object_to_json_string(new_obj));
	json_object* idObj;
	if (json_object_object_get_ex(new_obj, "Id", &idObj)) {
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
error_t docker_process_list_container(docker_context* ctx,
		docker_result** result, docker_container_ps**ps, char* id,
		char* process_args) {
	char* method = "/top";
	char* containers = "containers/";
	char* url = (char*) malloc(
			(strlen(containers) + strlen(id) + strlen(method) + 1)
					* sizeof(char));
	sprintf(url, "%s%s%s", containers, id, method);
	docker_log_debug("Top url is %s", url);

	json_object *new_obj;
	struct MemoryStruct chunk;
	docker_api_get(ctx, result, url, NULL, 0, &chunk);

	new_obj = json_tokener_parse(chunk.memory);
	docker_log_debug("Response = %s", json_object_to_json_string(new_obj));
	if (is_ok((*result))) {
		docker_container_ps* p = (*ps);
		p = (docker_container_ps*) malloc(sizeof(docker_container_ps));
		if (!p) {
			return E_ALLOC_FAILED;
		}
		json_object* titles_obj;
		json_object_object_get_ex(new_obj, "Titles", &titles_obj);
		p->num_titles = json_object_array_length(new_obj);
		p->titles = (char**) malloc(p->num_titles * sizeof(char*));
		for (int i = 0; i < p->num_titles; i++) {
			p->titles[i] = (char *) json_object_to_json_string(
					json_object_array_get_idx(titles_obj, i));
		}
	} else {
		json_object* msg_obj;
		json_object_object_get_ex(new_obj, "message", &msg_obj);
		(*result)->message = (char *) json_object_to_json_string(msg_obj);
	}

	return E_SUCCESS;
}

error_t docker_start_container(docker_context* ctx, docker_result** result,
		char* id) {
	char* method = "/start";
	char* containers = "containers/";
	char* url = (char*) malloc(
			(strlen(containers) + strlen(id) + strlen(method) + 1)
					* sizeof(char));
	sprintf(url, "%s%s%s", containers, id, method);
	docker_log_debug("Start url is %s", url);

	json_object *new_obj;
	struct MemoryStruct chunk;
	docker_api_post(ctx, result, url, NULL, 0, "", &chunk);

	new_obj = json_tokener_parse(chunk.memory);
	docker_log_debug("Response = %s", json_object_to_json_string(new_obj));

	return E_SUCCESS;
}

error_t docker_wait_container(docker_context* ctx, docker_result** result,
		char* id) {
	char* method = "/wait";
	char* containers = "containers/";
	char* url = (char*) malloc(
			(strlen(containers) + strlen(id) + strlen(method) + 1)
					* sizeof(char));
	sprintf(url, "%s%s%s", containers, id, method);
	docker_log_debug("Wait url is %s", url);

	json_object *new_obj;
	struct MemoryStruct chunk;
	docker_api_post(ctx, result, url, NULL, 0, "", &chunk);

	new_obj = json_tokener_parse(chunk.memory);
	docker_log_debug("Response = %s", json_object_to_json_string(new_obj));

	return E_SUCCESS;
}

error_t docker_stdout_container(docker_context* ctx, docker_result** result,
		char** log, char* id) {
	char* method = "/logs?stdout=1";
	char* containers = "containers/";
	char* url = (char*) malloc(
			(strlen(containers) + strlen(id) + strlen(method) + 1)
					* sizeof(char));
	sprintf(url, "%s%s%s", containers, id, method);
	docker_log_debug("Stdout url is %s", url);

	struct MemoryStruct chunk;
	docker_api_get(ctx, result, url, NULL, 0, &chunk);

	(*log) = chunk.memory + 8;
	return E_SUCCESS;
}

