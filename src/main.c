#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <json-c/json.h>

#include "docker_connection_util.h"
#include "docker_containers.h"

int main() {
	curl_global_init(CURL_GLOBAL_ALL);
    char* id;

    //docker_context* ctx = make_docker_context_url("http://192.168.1.33:2376/");
    docker_context* ctx = make_docker_context_socket("/var/run/docker.sock");

    docker_create_container_params* p = make_docker_create_container_params();
    p->image = "alpine";
    p->cmd = (char**)malloc(2 * sizeof(char*));
    p->cmd[0] = "echo";
    p->cmd[1] = "hello world";
    p->num_cmd = 2;
    id = docker_create_container(ctx, p);

    printf("Docker container id is %s\n", id);

    docker_start_container(ctx, id);
    docker_wait_container(ctx, id);
    docker_stdout_container(ctx, id);

	printf("\n\n========== Docker containers list.=========\n");

	docker_containers_list_filter* filter = make_docker_containers_list_filter();
	//containers_filter_add_name(filter, "/registryui");
	containers_filter_add_id(filter, id);
	docker_containers_list* containers = docker_container_list(ctx, 1, 5, 1, filter);
	printf("Read %d containers.\n", containers->num_containers);

	free_docker_context(ctx);
	curl_global_cleanup();
	return 0;

}
