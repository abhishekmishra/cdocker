/*
 * docker_containers.c
 *
 *  Created on: 11-Dec-2018
 *      Author: abhishek
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "docker_containers.h"
#include "docker_connection_util.h"

DockerContainersList* docker_containers_list() {
    char* method = "json";
    char* containers = "containers/";
    char* url = (char*) malloc((strlen(URL) + strlen(containers) + strlen(method) + 1) * sizeof(char));
    sprintf(url, "%s%s%s", URL, containers, method);
    printf("List url is %s\n", url);

    struct MemoryStruct chunk;
    docker_api_get(url, &chunk);

    //need to skip 8 bytes of binary junk
    printf("Output is \n%s\n", chunk.memory);
	return NULL;
}
