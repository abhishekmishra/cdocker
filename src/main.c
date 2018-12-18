#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <json-c/json.h>

#include "docker_result.h"
#include "docker_connection_util.h"
#include "docker_containers.h"
#include "log.h"

//TODO: create a basic error handling function, which can be reused.
void handle_error(docker_result* res) {
	log_info("DOCKER_RESULT: For URL: %s", get_url(res));
	log_info("DOCKER RESULT: Response error_code = %d, http_response = %ld", get_error(res),
			get_http_error(res));
	free_docker_result(&res);
}

int main() {
	curl_global_init(CURL_GLOBAL_ALL);
	char* id;

	docker_context* ctx;
	docker_result* res;

//	if (make_docker_context_url(&ctx, "http://192.168.1.33:2376/")
//			== E_SUCCESS) {
	if (make_docker_context_socket(&ctx, "/var/run/docker.sock") == E_SUCCESS) {
		docker_create_container_params* p;
		make_docker_create_container_params(&p);
		p->image = "alpine";
		p->cmd = (char**) malloc(2 * sizeof(char*));
		p->cmd[0] = "echo";
		p->cmd[1] = "hello world";
		p->num_cmd = 2;
		docker_create_container(ctx, &res, &id, p);
		handle_error(res);

		printf("Docker container id is %s\n", id);

		docker_start_container(ctx, &res, id);
		handle_error(res);

		docker_container_ps* ps;
		docker_process_list_container(ctx, &res, &ps, id, NULL);
		handle_error(res);

		docker_wait_container(ctx, &res, id);
		handle_error(res);

		char* log;
		docker_stdout_container(ctx, &res, &log, id);
		handle_error(res);

		printf("\n\n========== Docker containers list.=========\n");

		docker_containers_list_filter* filter;
		make_docker_containers_list_filter(&filter);
//		containers_filter_add_name(filter, "/registryui");
		containers_filter_add_id(filter, id);
		docker_containers_list* containers;
		docker_container_list(ctx, &res, &containers, 1, 5, 1, filter);
		handle_error(res);
		log_info("Read %d containers.\n", containers->num_containers);

		free_docker_context(&ctx);
	}
	curl_global_cleanup();
	return 0;

}
