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

DockerContainersList* docker_containers_list() {
    char* method = "json";
    char* containers = "containers/";
    char* url = (char*) malloc((strlen(URL) + strlen(containers) + strlen(method) + 1) * sizeof(char));
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

    for(int i = 0; i < containers_arr->length; i++) {
    	printf("Item #%d is %s\n", i, json_object_to_json_string(containers_arr->array[i]));
    }

	return NULL;
}
