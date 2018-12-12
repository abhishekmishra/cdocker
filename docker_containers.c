/*
 * docker_containers.c
 *
 *  Created on: 11-Dec-2018
 *      Author: abhishek
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include "docker_containers.h"
#include "docker_connection_util.h"

char* make_defensive_copy(const char* from) {
	char* to = (char*) malloc((strlen(from) + 1) * sizeof(char));
	strcpy(to, from);
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


DockerContainersList* docker_containers_list() {
	char* method = "json";
	char* containers = "containers/";
	char* url = (char*) malloc(
			(strlen(URL) + strlen(containers) + strlen(method) + 1)
					* sizeof(char));
	sprintf(url, "%s%s%s", URL, containers, method);
	printf("List url is %s\n", url);

	json_object *new_obj;
	struct MemoryStruct chunk;
	docker_api_get(url, &chunk);

	//need to skip 8 bytes of binary junk
	//printf("Output is \n%s\n", chunk.memory);

	new_obj = json_tokener_parse(chunk.memory);
	//printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
	struct array_list* containers_arr = json_object_get_array(new_obj);
	int len = containers_arr->length;

	for (int i = 0; i < containers_arr->length; i++) {
		printf("Item #%d is %s\n", i,
				json_object_to_json_string(containers_arr->array[i]));
		DockerContainersListItem* listItem = (DockerContainersListItem*) malloc(
				sizeof(DockerContainersListItem));

		listItem->id = get_attr_str(containers_arr->array[i], "Id");

		json_object* namesObj;
		if (json_object_object_get_ex(containers_arr->array[i], "Names",
				&namesObj)) {
			struct array_list* names_arr = json_object_get_array(namesObj);
			listItem->names = (char**) malloc(
					names_arr->length * sizeof(char*));
			for (int ni = 0; ni < names_arr->length; ni++) {
				listItem->names[ni] = make_defensive_copy(json_object_get_string(
						names_arr->array[ni]));
			}
			listItem->num_names = names_arr->length;
		}
		free(namesObj);

		listItem->image = get_attr_str(containers_arr->array[i], "Image");
		listItem->image_id = get_attr_str(containers_arr->array[i], "ImageID");
		listItem->command = get_attr_str(containers_arr->array[i], "Command");
		listItem->created = get_attr_long_long(containers_arr->array[i], "Created");
		listItem->state = get_attr_str(containers_arr->array[i], "State");
		listItem->status = get_attr_str(containers_arr->array[i], "Status");

		json_object* portsObj;
		if (json_object_object_get_ex(containers_arr->array[i], "Ports",
				&portsObj)) {
			struct array_list* ports_arr = json_object_get_array(portsObj);
			listItem->ports = (DockerContainerPorts**) malloc(
					ports_arr->length * sizeof(DockerContainerPorts*));
			listItem->num_ports = ports_arr->length;
			for (int ni = 0; ni < ports_arr->length; ni++) {
				listItem->ports[ni] = (DockerContainerPorts*) malloc(
						sizeof(DockerContainerPorts));

				listItem->ports[ni]->private_port = get_attr_int(ports_arr->array[ni], "PrivatePort");
				listItem->ports[ni]->public_port = get_attr_int(ports_arr->array[ni], "PublicPort");
				listItem->ports[ni]->type = get_attr_str(ports_arr->array[ni], "Type");
			}
		}
		free(portsObj);

		//TODO: Labels

		listItem->size_rw = get_attr_long_long(containers_arr->array[i], "SizeRW");
		listItem->size_root_fs = get_attr_long_long(containers_arr->array[i], "SizeRootFs");
	}

	return NULL;
}
