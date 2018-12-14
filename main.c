#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <json-c/json.h>

#include "docker_containers.h"

int main() {
	curl_global_init(CURL_GLOBAL_ALL);

    char* id;
    id = docker_create_container();

    printf("Docker container id is %s\n", id);

    docker_start_container(id);
    docker_wait_container(id);
    docker_stdout_container(id);

	printf("\n\n========== Docker containers list.=========\n");

	docker_containers_list_filter* filter = make_docker_containers_list_filter();
	//containers_filter_add_name(filter, "/registryui");
	containers_filter_add_id(filter, "7460166eeca46468a217d3fb058924f07bebb674a3502fa1b0440cd933cfc9ff");
	docker_containers_list* containers = docker_container_list(1, 5, 1, filter);
	printf("Read %d containers.\n", containers->num_containers);

	curl_global_cleanup();
	return 0;

}
