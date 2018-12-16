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

char* make_defensive_copy(const char* from) {
	char* to = NULL;
	if (strlen(from) > 0) {
		to = (char*) malloc((strlen(from) + 1) * sizeof(char));
		strcpy(to, from);
	}
	return to;
}

char* get_attr_str(json_object* obj, char* name) {
	json_object* extractObj;
	char* attr = NULL;
	if (json_object_object_get_ex(obj, name, &extractObj)) {
		attr = make_defensive_copy(json_object_get_string(extractObj));
		free(extractObj);
	}
	printf("%s is |%s|.\n\n", name, attr);
	return attr;
}

int get_attr_int(json_object* obj, char* name) {
	json_object* extractObj;
	int attr = -1;
	if (json_object_object_get_ex(obj, name, &extractObj)) {
		sscanf(json_object_get_string(extractObj), "%d", &attr);
		free(extractObj);
	}
	printf("%s is |%d|.\n\n", name, attr);
	return attr;
}

long long get_attr_long_long(json_object* obj, char* name) {
	json_object* extractObj;
	long long attr = -1;
	if (json_object_object_get_ex(obj, name, &extractObj)) {
		sscanf(json_object_get_string(extractObj), "%lld", &attr);
		free(extractObj);
	}
	printf("%s is |%lld|.\n\n", name, attr);
	return attr;
}

docker_containers_list_filter* make_docker_containers_list_filter() {
	docker_containers_list_filter* filter =
			(docker_containers_list_filter*) malloc(
					sizeof(docker_containers_list_filter));
	filter->num_ancestor = 0;
	filter->num_before = 0;
	filter->num_expose = 0;
	filter->num_exited = 0;
	filter->num_health = 0;
	filter->num_id = 0;
	filter->num_isolation = 0;
	filter->num_is_task = 0;
	filter->num_label = 0;
	filter->num_name = 0;
	filter->num_network = 0;
	filter->num_publish = 0;
	filter->num_since = 0;
	filter->num_status = 0;
	filter->num_volume = 0;
	return filter;
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

docker_containers_list* docker_container_list(int all, int limit, int size,
		docker_containers_list_filter* filters) {
	char* method = "json";
	char* containers = "containers/";
	char* url = (char*) malloc(
			(strlen(URL) + strlen(containers) + strlen(method) + 1)
					* sizeof(char));
	sprintf(url, "%s%s%s", URL, containers, method);
	printf("List url is %s\n", url);

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
	docker_api_get(url, params, num_params, &chunk);
	free(params);

	//need to skip 8 bytes of binary junk
	//printf("Output is \n%s\n", chunk.memory);

	new_obj = json_tokener_parse(chunk.memory);
	//printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
	struct array_list* containers_arr = json_object_get_array(new_obj);
	int len = containers_arr->length;

	docker_containers_list* clist = (docker_containers_list*) malloc(
			sizeof(docker_containers_list));
	clist->num_containers = len;
	clist->containers = (docker_containers_list_item**) malloc(
			len * sizeof(docker_containers_list_item));

	for (int i = 0; i < containers_arr->length; i++) {
		printf("Item #%d is %s\n", i,
				json_object_to_json_string(containers_arr->array[i]));
		docker_containers_list_item* listItem =
				(docker_containers_list_item*) malloc(
						sizeof(docker_containers_list_item));
		clist->containers[i] = listItem;
		listItem->id = get_attr_str(containers_arr->array[i], "Id");

		json_object* namesObj;
		if (json_object_object_get_ex(containers_arr->array[i], "Names",
				&namesObj)) {
			struct array_list* names_arr = json_object_get_array(namesObj);
			listItem->names = (char**) malloc(
					names_arr->length * sizeof(char*));
			for (int ni = 0; ni < names_arr->length; ni++) {
				listItem->names[ni] = make_defensive_copy(
						json_object_get_string(names_arr->array[ni]));
			}
			listItem->num_names = names_arr->length;
		}
		free(namesObj);

		listItem->image = get_attr_str(containers_arr->array[i], "Image");
		listItem->image_id = get_attr_str(containers_arr->array[i], "ImageID");
		listItem->command = get_attr_str(containers_arr->array[i], "Command");
		listItem->created = get_attr_long_long(containers_arr->array[i],
				"Created");
		listItem->state = get_attr_str(containers_arr->array[i], "State");
		listItem->status = get_attr_str(containers_arr->array[i], "Status");

		json_object* portsObj;
		if (json_object_object_get_ex(containers_arr->array[i], "Ports",
				&portsObj)) {
			struct array_list* ports_arr = json_object_get_array(portsObj);
			listItem->ports = (docker_container_ports**) malloc(
					ports_arr->length * sizeof(docker_container_ports*));
			listItem->num_ports = ports_arr->length;
			for (int ni = 0; ni < ports_arr->length; ni++) {
				listItem->ports[ni] = (docker_container_ports*) malloc(
						sizeof(docker_container_ports));

				listItem->ports[ni]->private_port = get_attr_int(
						ports_arr->array[ni], "PrivatePort");
				listItem->ports[ni]->public_port = get_attr_int(
						ports_arr->array[ni], "PublicPort");
				listItem->ports[ni]->type = get_attr_str(ports_arr->array[ni],
						"Type");
			}
		}
		free(portsObj);

		json_object* labelsObj;
		if (json_object_object_get_ex(containers_arr->array[i], "Labels",
				&labelsObj)) {
			listItem->num_labels = 0;
			json_object_object_foreach(labelsObj, key, val) {
				listItem->num_labels += 1;
			}
			printf("Num labels is %d\n", listItem->num_labels);
			listItem->labels = (docker_container_label**) malloc(
					listItem->num_labels * sizeof(docker_container_label*));
			int lbl_count = 0;
			json_object_object_foreach(labelsObj, key1, val1)
			{
				listItem->labels[lbl_count] = (docker_container_label*) malloc(
						sizeof(docker_container_label));
				listItem->labels[lbl_count]->key = make_defensive_copy(key1);
				listItem->labels[lbl_count]->value = make_defensive_copy(
						json_object_get_string((json_object *) val1));
				printf("Label [%s] = [%s]\n", listItem->labels[lbl_count]->key,
						listItem->labels[lbl_count]->value);
				lbl_count += 1;
			}
		}
		free(labelsObj);

		listItem->size_rw = get_attr_long_long(containers_arr->array[i],
				"SizeRW");
		listItem->size_root_fs = get_attr_long_long(containers_arr->array[i],
				"SizeRootFs");

		json_object* hostConfigObj;
		if (json_object_object_get_ex(containers_arr->array[i], "HostConfig",
				&hostConfigObj)) {
			listItem->hostConfig = (docker_container_host_config*) malloc(
					sizeof(docker_container_host_config));
			listItem->hostConfig->network_mode = get_attr_str(hostConfigObj,
					"NetworkMode");
		}
		free(hostConfigObj);

		json_object* networkSettingsObj;
		if (json_object_object_get_ex(containers_arr->array[i],
				"NetworkSettings", &networkSettingsObj)) {
			json_object* networksObj;
			if (json_object_object_get_ex(networkSettingsObj, "Networks",
					&networksObj)) {
				listItem->network_settings =
						(docker_container_network_settings*) malloc(
								sizeof(docker_container_network_settings));
				listItem->network_settings->num_network_items = 0;
				json_object_object_foreach(networksObj, key, val) {
					listItem->network_settings->num_network_items += 1;
				}
				printf("Num networks is %d\n",
						listItem->network_settings->num_network_items);
				listItem->network_settings->network_items =
						(docker_container_network_settings_network**) malloc(
								listItem->network_settings->num_network_items
										* sizeof(docker_container_network_settings_network*));
				int ns_count = 0;
				json_object_object_foreach(networksObj, k, v) {
					listItem->network_settings->network_items[ns_count] =
							(docker_container_network_settings_network*) malloc(
									sizeof(docker_container_network_settings_network));
					listItem->network_settings->network_items[ns_count]->name =
							make_defensive_copy(k);
					listItem->network_settings->network_items[ns_count]->item =
							(docker_container_network_settings_network_item*) malloc(
									sizeof(docker_container_network_settings_network_item));
					listItem->network_settings->network_items[ns_count]->item->network_id =
							get_attr_str(v, "NetworkID");
					listItem->network_settings->network_items[ns_count]->item->endpoint_id =
							get_attr_str(v, "EndpointID");
					listItem->network_settings->network_items[ns_count]->item->gateway =
							get_attr_str(v, "Gateway");
					listItem->network_settings->network_items[ns_count]->item->ip_address =
							get_attr_str(v, "IPAddress");
					listItem->network_settings->network_items[ns_count]->item->ip_prefix_len =
							get_attr_int(v, "IPPrefixLen");
					listItem->network_settings->network_items[ns_count]->item->ipv6_gateway =
							get_attr_str(v, "IPv6Gateway");
					listItem->network_settings->network_items[ns_count]->item->global_ipv6_address =
							get_attr_str(v, "GlobalIPv6Address");
					listItem->network_settings->network_items[ns_count]->item->global_ipv6_prefix_len =
							get_attr_int(v, "GlobalIPv6PrefixLen");
					listItem->network_settings->network_items[ns_count]->item->mac_address =
							get_attr_str(v, "MacAddress");
					ns_count++;
				}
				free(networksObj);
			}
			free(networkSettingsObj);
		}

		json_object* mountsObj;
		if (json_object_object_get_ex(containers_arr->array[i], "Mounts",
				&mountsObj)) {
			struct array_list* mounts_arr = json_object_get_array(mountsObj);
			listItem->mounts = (docker_container_mount**) malloc(
					mounts_arr->length * sizeof(docker_container_mount*));
			for (int ni = 0; ni < mounts_arr->length; ni++) {
				listItem->mounts[ni] = (docker_container_mount*) malloc(
						sizeof(docker_container_mount));
				listItem->mounts[ni]->name = get_attr_str(mounts_arr->array[ni],
						"Name");
				listItem->mounts[ni]->type = get_attr_str(mounts_arr->array[ni],
						"Type");
				listItem->mounts[ni]->source = get_attr_str(
						mounts_arr->array[ni], "Source");
				listItem->mounts[ni]->destination = get_attr_str(
						mounts_arr->array[ni], "Destination");
				listItem->mounts[ni]->driver = get_attr_str(
						mounts_arr->array[ni], "Driver");
				listItem->mounts[ni]->mode = get_attr_str(mounts_arr->array[ni],
						"Mode");
				listItem->mounts[ni]->rw = get_attr_int(mounts_arr->array[ni],
						"Read");
				listItem->mounts[ni]->propagation = get_attr_str(
						mounts_arr->array[ni], "Propagation");
			}
			listItem->num_names = mounts_arr->length;
			free(mountsObj);
		}
	}

	free(url);
	return clist;
}

docker_create_container_params* make_docker_create_container_params() {
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
	return p;
}

char* docker_create_container(docker_create_container_params* params) {
	char* id = NULL;
	json_object *new_obj;
	struct MemoryStruct chunk;

	char* method = "create";
	char* containers = "containers/";
	char* url = (char*) malloc(
			(strlen(URL) + strlen(containers) + strlen(method) + 1)
					* sizeof(char));
	sprintf(url, "%s%s%s", URL, containers, method);
	printf("Start url is %s\n", url);

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

//	docker_api_post(url, NULL, 0,
//			"{\"Image\": \"alpine\", \"Cmd\": [\"echo\", \"hello world\"]}",
//			&chunk);
	docker_api_post(url, NULL, 0,
			json_object_to_json_string(create_obj),
			&chunk);

	new_obj = json_tokener_parse(chunk.memory);
	printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
	json_object* idObj;
	if (json_object_object_get_ex(new_obj, "Id", &idObj)) {
		const char* container_id = json_object_get_string(idObj);
		id = (char*) malloc((strlen(container_id) + 1) * sizeof(char));
		strcpy(id, container_id);
	} else {
		printf("Id not found.");
	}
	free(chunk.memory);
	return id;
}

int docker_start_container(char* id) {
	char* method = "/start";
	char* containers = "containers/";
	char* url = (char*) malloc(
			(strlen(URL) + strlen(containers) + strlen(id) + strlen(method) + 1)
					* sizeof(char));
	sprintf(url, "%s%s%s%s", URL, containers, id, method);
	printf("Start url is %s\n", url);

	json_object *new_obj;
	struct MemoryStruct chunk;
	docker_api_post(url, NULL, 0, "", &chunk);

	new_obj = json_tokener_parse(chunk.memory);
	printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));

	return 0;
}

int docker_wait_container(char* id) {
	char* method = "/wait";
	char* containers = "containers/";
	char* url = (char*) malloc(
			(strlen(URL) + strlen(containers) + strlen(id) + strlen(method) + 1)
					* sizeof(char));
	sprintf(url, "%s%s%s%s", URL, containers, id, method);
	printf("Wait url is %s\n", url);

	json_object *new_obj;
	struct MemoryStruct chunk;
	docker_api_post(url, NULL, 0, "", &chunk);

	new_obj = json_tokener_parse(chunk.memory);
	printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));

	return 0;
}

int docker_stdout_container(char* id) {
	char* method = "/logs?stdout=1";
	char* containers = "containers/";
	char* url = (char*) malloc(
			(strlen(URL) + strlen(containers) + strlen(id) + strlen(method) + 1)
					* sizeof(char));
	sprintf(url, "%s%s%s%s", URL, containers, id, method);
	printf("Stdout url is %s\n", url);

	struct MemoryStruct chunk;
	docker_api_get(url, NULL, 0, &chunk);

	//need to skip 8 bytes of binary junk
	printf("Output is \n%s\n", chunk.memory + 8);

	return 0;
}

