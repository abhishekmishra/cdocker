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

	    json_object* idObj;
	    if (json_object_object_get_ex(containers_arr->array[i], "Id", &idObj)) {
	        const char* container_id = json_object_get_string(idObj);
	        listItem->id = (char*)malloc((strlen(container_id) + 1) * sizeof(char));
	        strcpy(listItem->id, container_id);
	    }
	    free(idObj);
	    printf("Container id is %s.\n\n", listItem->id);

	    json_object* namesObj;
	    if (json_object_object_get_ex(containers_arr->array[i], "Names", &namesObj)) {
	    	struct array_list* names_arr = json_object_get_array(namesObj);
	    	listItem->names = (char**)malloc(names_arr->length * sizeof(char*));
	    	for(int ni = 0; ni < names_arr->length; ni++) {
		        const char* name_str = json_object_get_string(names_arr->array[ni]);
		        printf("%s\n", name_str);
		        listItem->names[ni] = (char*)malloc((strlen(name_str) + 1) * sizeof(char));
		        strcpy(listItem->names[ni], name_str);
	    	}
	    	listItem->num_names = names_arr->length;
	    }
	    free(namesObj);

	    json_object* imageObj;
	    if (json_object_object_get_ex(containers_arr->array[i], "Image", &imageObj)) {
	        const char* image = json_object_get_string(imageObj);
	        listItem->image = (char*)malloc((strlen(image) + 1) * sizeof(char));
	        strcpy(listItem->image, image);
	    }
	    free(imageObj);
	    printf("Image is %s.\n\n", listItem->image);

	    json_object* imageIdObj;
	    if (json_object_object_get_ex(containers_arr->array[i], "ImageID", &imageIdObj)) {
	        const char* imageId = json_object_get_string(imageIdObj);
	        listItem->image_id = (char*)malloc((strlen(imageId) + 1) * sizeof(char));
	        strcpy(listItem->image_id, imageId);
	    }
	    free(imageIdObj);
	    printf("Image ID is %s.\n\n", listItem->image_id);

	    json_object* commandObj;
	    if (json_object_object_get_ex(containers_arr->array[i], "Command", &commandObj)) {
	        const char* command = json_object_get_string(commandObj);
	        listItem->command = (char*)malloc((strlen(command) + 1) * sizeof(char));
	        strcpy(listItem->command, command);
	    }
	    free(commandObj);
	    printf("Command is %s.\n\n", listItem->command);

	    json_object* createdObj;
	    if (json_object_object_get_ex(containers_arr->array[i], "Created", &createdObj)) {
	        const char* created = json_object_get_string(createdObj);
	        sscanf(created, "%d", &(listItem->created));
	    }
	    free(createdObj);
	    printf("Created is %ld.\n\n", listItem->created);

//	    json_object* commandObj;
//	    if (json_object_object_get_ex(containers_arr->array[i], "Command", &commandObj)) {
//
//	    }
	}

	return NULL;
}
